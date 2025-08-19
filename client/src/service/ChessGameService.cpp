// ChessGameService.cpp

#include <boost/assert.hpp>

#include "ChessGameService.h"
#include "ChessGameUtils.h"
#include "GeminiAgent.h"

#include "base/Logger.h"
#include "model/Piece.h"
#include "model/chess/ChessHelpers.h"
#include "model/chess/IChessRule.h"

namespace bgg
{
  ChessGameService::ChessGameService(
      std::string apiKey,
      std::shared_ptr<IChessRule> chessRule,
      Side const &agentColor,
      std::function<void(ServerMessage const &)> messageSender) noexcept
      : agent_(std::make_shared<GeminiAgent>(
            std::move(apiKey),
            chess::AGENT_SYSTEM_INSTRUCTION,
            chess::MOVE_JSON_SCHEMA,
            chess::PROMPT_PATTERN)),
        agentColor_(std::move(agentColor)),
        opponentColor_(chess::getOpponentColor(agentColor_)),
        maxPromptRetries_(15),
        chessRule_(std::move(chessRule)),
        messageSender_(std::move(messageSender)),
        gameFinished_(false)
  {
    BOOST_ASSERT_MSG(
        agent_ != nullptr,
        "GeminiAgent must not be null in ChessGameService.");

    BOOST_ASSERT_MSG(
        chessRule_ != nullptr,
        "IChessRule must not be null in ChessGameService.");

    BOOST_ASSERT_MSG(
        messageSender_ != nullptr,
        "Message sender must not be null in ChessGameService.");
  }

  ChessGameService::~ChessGameService()
  {
    gameFinished_ = true;

    std::lock_guard<std::mutex> lock(mutexForThis_);
    SPDLOG_DEBUG("ChessGameService has been destroyed.");
  }

  void ChessGameService::handleRequest(ResignGameRequest const & /*request*/)
  {
    gameFinished_ = true;
    messageSender_(GameFinishedNotification{"Lose"});
  }

  void ChessGameService::handleRequest(MoveRequest const &moveRqt)
  {
    std::lock_guard<std::mutex> lock(mutexForThis_);
    ///< Validate the move request
    if (moveRqt.move)
    {
      ChessMove::Detail moveDetail = chessRule_->tryMove(moveRqt.move.value());
      try
      {
        messageSender_(MoveResponse{ErrorCode{0, "MoveValidation", ""}});
        chessRule_->commitMove(moveDetail);

        if (ChessMove::getOpponentKingStatus(moveDetail) == Side::Status::CHECKMATED)
        {
          messageSender_(GameFinishedNotification{"Win"});
          return; // Game is finished, no need to send prompt to agent
        }

        if (ChessMove::getOpponentKingStatus(moveDetail) == Side::Status::STALEMATED)
        {
          messageSender_(GameFinishedNotification{"Draw"});
          return; // Game is finished, no need to send prompt to agent
        }
      }
      catch (std::exception const &e)
      {
        messageSender_(MoveResponse{ErrorCode{-1, "MoveValidation", e.what()}});
        return;
      }
    }

    sendMoveRequestToAgent();
  }

  void ChessGameService::sendMoveRequestToAgent()
  {
    auto agentPieceList = chessRule_->collectPieces(agentColor_);
    auto agentPiecePlacementsStr = piecePlacementsToJsonStr(agentPieceList);

    auto opponentPieceList = chessRule_->collectPieces(opponentColor_);
    auto opponentPiecePlacementsStr = piecePlacementsToJsonStr(opponentPieceList);

    // TODO: handle the error case when there are no piece placements
    // emit(MoveResponse{ErrorCode{-1, "Mock", "No piece placements found"}});

    ///< send the prompt to the agent]
    auto lastMove = chessRule_->getLastMove();
    std::string opponentLastMove = "null";
    if (lastMove)
    {
      opponentLastMove = chessMoveDetailToJsonStr(*lastMove);
    }
    std::string pastFailureMessages;

    for (int i = 0; i < maxPromptRetries_; ++i)
    {
      if (gameFinished_)
      {
        SPDLOG_DEBUG("Game is finished, stopping prompt attempts.");
        return;
      }

      std::string input = std::format(
          R"(
          {{
            "yourColor": "{}",
            "opponentLastMove": {},
            "opponentPiecePlacements": {{ {} }},
            "yourPiecePlacements": {{ {} }},
            "pastFailureMessages": [ {} ]
          }})",
          agentColor_.toString(),
          opponentLastMove,
          opponentPiecePlacementsStr,
          agentPiecePlacementsStr,
          pastFailureMessages);

      SPDLOG_DEBUG("Sending prompt to agent: {}", input);

      std::optional<std::string> agentResponse = agent_->sendPromptWithArgs(input);
      if (!agentResponse)
      {
        SPDLOG_WARN(
            "Prompt failure number: {} (connection error).\nRetrying...",
            i + 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        continue;
      }

      ChessMove agentMove;
      // std::string uicResponseMove;
      try
      {
        agentMove = chess::jsonToChessMove(*agentResponse);
        agentMove.color = agentColor_;
      }
      catch (std::exception const &e)
      {
        SPDLOG_WARN(
            "Prompt failure number: {} (JSON parsing failure: {}).\nRetrying...",
            i + 1,
            e.what());
        // TODO: consider adding this failure to pastFailureMessages
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        continue;
      }

      ChessMove::Detail agentMoveDetail{};
      try
      {
        agentMoveDetail = chessRule_->tryMove(agentMove);
      }
      catch (std::exception const &e)
      {
        ///< handle the error case when 'agentMove' is invalid
        auto promotedPiece = ChessMove::getPromotedPieceType(agentMoveDetail);
        std::string promotionRelatedMsg;
        if (promotedPiece)
        {
          promotionRelatedMsg =
              " and promoted that piece to " + promotedPiece->toString();
        }

        std::string failureMsg = std::format(
            "\"You want to move a piece from {} to {}{}. But... {}\"",
            agentMove.fromSquare.toString(),
            agentMove.toSquare.toString(),
            promotionRelatedMsg,
            e.what());

        if (pastFailureMessages.empty())
        {
          pastFailureMessages = failureMsg;
        }
        else
        {
          pastFailureMessages += ",\n" + failureMsg;
        }

        SPDLOG_WARN(
            "Prompt failure number: {} \n(rule violation:\n{}).\nRetrying...",
            i + 1,
            pastFailureMessages);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        continue;
      }

      // moveHistoryStr_ += ",\"" + uicResponseMove + "\"";
      chessRule_->commitMove(agentMoveDetail);

      Side currentTurn = chess::getOpponentColor(agentColor_);
      messageSender_(
          GameUpdatedNotification{agentMoveDetail, currentTurn, currentTurn});

      if (ChessMove::getOpponentKingStatus(agentMoveDetail) ==
          Side::Status::CHECKMATED)
      {
        messageSender_(GameFinishedNotification{"Lose"});
      }
      return;
    }

    messageSender_(GameFinishedNotification{"Error"});
    SPDLOG_ERROR("Completely failed to send prompt to the agent");
  }

  auto ChessGameService::chessMoveDetailToJsonStr(
      ChessMove::Detail const &moveDetail) noexcept -> std::string
  {
    if (auto normalMove = moveDetail.getIf<ChessMove::Normal>())
    {
      std::string capturedPiece = "null";
      if (normalMove->capturedPiece)
      {
        capturedPiece = "\"" + normalMove->capturedPiece->toString() + "\"";
      }
      return std::format(
          R"(
          {{
            "type": "NormalMove",
            "moveNumber": {},
            "color": {},
            "movedPiece": "{}",
            "fromSquare": "{}",
            "toSquare": "{}",
            "yourCapturedPiece": {},
            "yourKingSafety": "{}"
          }})",
          normalMove->moveNumber,
          normalMove->color.toString(),
          normalMove->movedPiece.toString(),
          normalMove->fromSquare.toString(),
          normalMove->toSquare.toString(),
          capturedPiece,
          Side::toString(normalMove->opponentKingStatus));
    }
    else if (auto promotion = moveDetail.getIf<ChessMove::Promotion>())
    {
      std::string capturedPiece = "null";
      if (promotion->capturedPiece)
      {
        capturedPiece = "\"" + promotion->capturedPiece->toString() + "\"";
      }
      return std::format(
          R"(
          {{
            "type": "Promotion",
            "moveNumber": {},
            "color": {},
            "movedPiece": "{}",
            "fromSquare": "{}",
            "toSquare": "{}",
            "promotedPiece": "{}",
            "yourCapturedPiece": {},
            "yourKingSafety": "{}"
          }})",
          promotion->moveNumber,
          promotion->color.toString(),
          promotion->movedPiece.toString(),
          promotion->fromSquare.toString(),
          promotion->toSquare.toString(),
          promotion->promotedPiece.toString(),
          capturedPiece,
          Side::toString(promotion->opponentKingStatus));
    }
    else if (auto enPassantCapture = moveDetail.getIf<ChessMove::EnPassant>())
    {
      return std::format(
          R"(
          {{
            "type": "EnPassant",
            "moveNumber": {},
            "color": {},
            "movedPiece": "{}",
            "fromSquare": "{}",
            "toSquare": "{}",
            "enPassantCaptureSquare": "{}",
            "yourCapturedPiece": "Pawn",
            "yourKingSafety": "{}"
          }})",
          enPassantCapture->moveNumber,
          enPassantCapture->color.toString(),
          enPassantCapture->movedPiece.toString(),
          enPassantCapture->fromSquare.toString(),
          enPassantCapture->toSquare.toString(),
          enPassantCapture->enPassantCaptureSquare.toString(),
          Side::toString(enPassantCapture->opponentKingStatus));
    }
    else if (auto castling = moveDetail.getIf<ChessMove::Castling>())
    {
      return std::format(
          R"(
          {{
            "type": "Castling",
            "moveNumber": {},
            "color": {},
            "movedPiece": "{}",
            "fromSquare": "{}",
            "toSquare": "{}",
            "rookSource": "{}",
            "rookDestination": "{}",
            "yourKingSafety": "{}"
          }})",
          castling->moveNumber,
          castling->color.toString(),
          castling->movedPiece.toString(),
          castling->fromSquare.toString(),
          castling->toSquare.toString(),
          castling->rookSource.toString(),
          castling->rookDestination.toString(),
          Side::toString(castling->opponentKingStatus));
    }
    else // if (auto emptyAction )
    {
      return "null";
    }
  }

  auto ChessGameService::piecePlacementsToJsonStr(
      std::list<std::shared_ptr<Piece>> const &piecePlacements) noexcept -> std::string
  {
    if (piecePlacements.empty())
    {
      SPDLOG_ERROR("No piece placements to convert to JSON string");
      return "";
    }
    std::string piecePlacementsStr;
    for (auto const &piece : piecePlacements)
    {
      piecePlacementsStr += std::format(
          R"("{}":"{}",)",
          piece->getPosition().toString(),
          piece->getSide().toString() + piece->getType().toString());
    }
    piecePlacementsStr.pop_back(); // remove the last comma

    return piecePlacementsStr;
  }
} // namespace bgg
