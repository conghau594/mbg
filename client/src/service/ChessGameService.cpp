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
  constexpr const char CHESS_GAME_SYSTEM_INSTRUCTION[] =
      R"(
        You are a master of chess. You play using the UCI  protocol.
        You will be given the `color` of the pieces you are controlling.
        You will then receive the `piecePlacements` of all pieces on the board.
        You will also receive the `opponentLastMove`.
        
        Using this information, figure out the current board state, understand 
        your opponent's intentions through its last move, and respond concisely
        with your next move in the following format. 
        {
          "purpose": "<brief reason why you do this move (at most 3 sentences)>",
          "fromSquare": "<source square>",
          "toSquare": "<destination square>",
          "promote": "<promoted piece if needed>"
        }
        For example:
        - A rook moves from a1 to a3 normally:
          {
            "purpose": "...",
            "fromSquare": "a1",
            "toSquare": "a3"
          }
        - A black pawn from a2 reachs to the last rank at a1 and promotes to 
          a queen:
          {
            "purpose": "...",
            "fromSquare": "a2",
            "toSquare": "a1",
            "promote": "Queen"
          }

        If you want to castle or capture en passant, you just specify the
        `fromSquare` and the `toSquare` of the moved piece.
        For example:
        - The black king castles at queen side. So it moves from e8 to c8:
          {
            "purpose": "...",
            "fromSquare": "e8",
            "toSquare": "c8"
          }
        - An white pawn from e5 captures en passant a black pawn at d5. So it 
          moves to d6:
          {
            "purpose": "...",
            "fromSquare": "e5",
            "toSquare": "d6"
          }
        - And so on.

        Sometimes, you make invalid moves. All your failures in the past is
        stored in `pastFailureMessages`. You should take it as a lesson then
        try again.

        Make sure to:
        - Always evaluate the safety of your king before making any move.
        - Ensure your move is valid and keeps your king out of check.

        <EXAMPLE>
          INPUT: 
            {
              "yourColor": "Black",
              "currentPlacments": {
                "a1": "WhiteRook", "b1": "WhiteKnight", "c1": "WhiteBishop", "d1": "WhiteQueen",
                "e1": "WhiteKing", "f1": "WhiteBishop", "g1": "WhiteKnight", "h1": "WhiteRook",
                "a2": "WhitePawn", "b2": "WhitePawn",   "c2": "WhitePawn",   "d2": "WhitePawn",
                "e4": "WhitePawn", "f2": "WhitePawn",   "g2": "WhitePawn",   "h2": "WhitePawn",
                "a8": "BlackRook", "b8": "BlackKnight", "c8": "BlackBishop", "d8": "BlackQueen",
                "e8": "BlackKing", "f8": "BlackBishop", "g8": "BlackKnight", "h8": "BlackRook",
                "a7": "BlackPawn", "b7": "BlackPawn",   "c7": "BlackPawn",   "d7": "BlackPawn",
                "e7": "BlackPawn", "f7": "BlackPawn",   "g7": "BlackPawn",   "h7": "BlackPawn"
              },
              "pastFailureMessages": [],
              "opponentLastMove": {
                "type": "Normal",
                "piece": "White Pawn",
                "fromSquare": "e2",
                "toSquare": "e4",
                "capture": false,
                "yourKingSafety": "Safe"
              }
            } 
          OUTPUT:
            {
              "purpose": "I want to contest the center by mirroring White's e4 move. 
                          This is a classical and strong response that opens lines 
                          for the queen and bishop.",
              "fromSquare": "e7",
              "toSquare": "e5"
            }
        </EXAMPLE>

        <EXAMPLE>
          INPUT: 
            {
              "yourColor": "Black",
              "currentPlacments": {
                "a1": "WhiteRook", "b1": "WhiteKnight", "c1": "WhiteBishop", "d1": "WhiteQueen",
                "e1": "WhiteKing", "f1": "WhiteBishop", "f3": "WhiteKnight", "h1": "WhiteRook",
                "a2": "WhitePawn", "b2": "WhitePawn",   "c2": "WhitePawn",   "d2": "WhitePawn",
                "e4": "WhitePawn", "f2": "WhitePawn",   "g2": "WhitePawn",   "h2": "WhitePawn",
                "a8": "BlackRook", "b8": "BlackKnight", "c8": "BlackBishop", "d8": "BlackQueen",
                "e8": "BlackKing", "f8": "BlackBishop", "g8": "BlackKnight", "h8": "BlackRook",
                "a7": "BlackPawn", "b7": "BlackPawn",   "c7": "BlackPawn",   "d7": "BlackPawn",
                "e5": "BlackPawn", "f7": "BlackPawn",   "g7": "BlackPawn",   "h7": "BlackPawn"
              },
              "pastFailureMessages": [
                {
                  "illegalMove": {
                    "fromSquare": "b8",
                    "toSquare": "c5"
                  },
                  "message": "Invalid destination square"
                }
              ],
              "opponentLastMove": {
                "type": "Normal",
                "piece": "White Pawn",
                "fromSquare": "g1",
                "toSquare": "f3",
                "capture": false,
                "yourKingSafety": "Safe"
              }
            }
          OUTPUT:
            {
              "purpose": "I develop the knight to a natural square to control
                          the center (d4 and e5), and prepare for kingside castling.",
              "fromSquare": "b8",
              "toSquare": "c6",
              "promote": null
            }
        </EXAMPLE>

        <EXAMPLE>
          INPUT:
            {  
              "yourColor": "White",
              "currentPlacments": {
                "a1": "WhiteRook", "b1": "WhiteKnight", "c1": "WhiteBishop", "d1": "WhiteQueen",
                "e1": "WhiteKing", "f1": "WhiteBishop", "g1": "WhiteKnight", "h1": "WhiteRook",
                "a2": "WhitePawn", "b2": "WhitePawn",   "c2": "WhitePawn",   "d2": "WhitePawn",
                "e4": "WhitePawn", "f2": "WhitePawn",   "g2": "WhitePawn",   "h2": "WhitePawn",
                "a8": "BlackRook", "b8": "BlackKnight", "c8": "BlackBishop", "d8": "BlackQueen",
                "e8": "BlackKing", "f8": "BlackBishop", "g8": "BlackKnight", "h8": "BlackRook",
                "a7": "BlackPawn", "b7": "BlackPawn",   "c7": "BlackPawn",   "d7": "BlackPawn",
                "e7": "BlackPawn", "f7": "BlackPawn",   "g7": "BlackPawn",   "h7": "BlackPawn"
              },
              "pastFailureMessages": [
                {
                  "illegalMove": {
                    "fromSquare": "e3",
                    "toSquare": "e4",
                    "promote": null
                  },
                  "message": "Invalid source square"
                }
              ],
              "opponentLastMove": null
            }
          OUTPUT:
            {
              "purpose": "I open with the King's Pawn controls the center and 
                          opens lines for the queen and bishop. It's a classical 
                          and strong first move.",
              "fromSquare": "e2",
              "toSquare": "e4"
            }
        </EXAMPLE>
      )";

  constexpr const char CHESS_GAME_PROMPT_PATTERN[] = "{}";

  constexpr const char CHESS_MOVE_JSON_SCHEMA[] =
      R"(
        {
          "type": "object",
          "properties": {
            "purpose": {
              "type": "string"
            },
            "fromSquare": {
              "type": "string",
              "pattern": "^[a-h][1-8]$"
            },
            "toSquare": {
              "type": "string",
              "pattern": "^[a-h][1-8]$"
            },
            "promote": {
              "type": "string",
              "enum": ["Queen", "Rook", "Bishop", "Knight"],
              "nullable": true,
              "default": null
            }
          },
          "required": ["purpose", "fromSquare", "toSquare"],
          "propertyOrdering": ["purpose", "fromSquare", "toSquare", "promote"]
        }
      )";

  ChessGameService::ChessGameService(
      std::string apiKey,
      std::shared_ptr<IChessRule> chessRule,
      Side const &agentColor,
      std::shared_ptr<ClientEventBus> eventBus)
      : threadPool_(2),
        eventBus_(std::move(eventBus)),
        subscriptionIdList_{},
        agent_(std::make_shared<GeminiAgent>(
            std::move(apiKey),
            CHESS_GAME_SYSTEM_INSTRUCTION,
            CHESS_MOVE_JSON_SCHEMA,
            CHESS_GAME_PROMPT_PATTERN)),
        agentColor_(std::move(agentColor)),
        allyColor_(chess::getOpponentColor(agentColor_)),
        maxPromptRetries_(10),
        chessRule_(std::move(chessRule))
  // moveHistoryStr_{}
  {
    auto subscriptionId = eventBus_->subscribe<ClientRequest, MoveRequest>(
        [this](MoveRequest const &moveRqt)
        {
          threadPool_.push(
              [this, moveRqt]()
              {
                std::lock_guard<std::mutex> lock(mutexForThis_);
                ///< Validate the move request
                bool gameFinished;
                try
                {
                  validateMoveRequest(moveRqt, gameFinished);
                }
                catch (std::exception const &e)
                {
                  int errCodeValue = -1;
                  emit(MoveResponse{ErrorCode{errCodeValue, "Mock", e.what()}});
                  return;
                }

                if (gameFinished)
                {
                  return; // Game is finished, no need to send prompt to agent
                }
                sendChessGamePromptToAgent();
              });
        });

    if (!subscriptionId)
    {
      std::string msg = std::format(
          "Cannot subscribe the client request of 'MoveRequest' from '{}'",
          typeid(*this).name());
      throw(std::runtime_error(msg));
    }
    subscriptionIdList_.emplace_back(*subscriptionId);

    SPDLOG_INFO("'{}' has subscribed to client requests of type 'MoveRequest'",
                typeid(*this).name());
  }

  ChessGameService::~ChessGameService()
  {
    maxPromptRetries_ = 0; // stop any further attempts to send prompts
    for (auto &id : subscriptionIdList_)
    {
      eventBus_->unsubscribe<ClientRequest>(id);
    }
    std::lock_guard<std::mutex> lock(mutexForThis_);
  }

  void ChessGameService::emit(ServerMessage const &msg) noexcept
  {
    eventBus_->emit<ServerMessage>(msg);
  }

  void ChessGameService::validateMoveRequest(MoveRequest const &moveRqt, bool &gameFinished)
  {
    ChessMove::Detail moveDetail = chessRule_->tryMove(moveRqt.move);

    if (moveDetail.is<ChessMove::Invalid>())
    {
      throw std::invalid_argument(std::format(
          "Invalid move request: from {} to {} for color {}",
          moveRqt.move.fromSquare.toString(),
          moveRqt.move.fromSquare.toString(),
          allyColor_.toString()));
    }

    emit(MoveResponse{ErrorCode{0, "Mock", ""}});
    chessRule_->commitMove(moveDetail);

    if (ChessMove::getOpponentKingStatus(moveDetail) == Side::Status::CHECKMATED)
    {
      emit(GameFinishedNotification{"Win"});
      gameFinished = true;
    }
    else
    {
      gameFinished = false;
    }
  }

  void ChessGameService::sendChessGamePromptToAgent()
  {
    std::string agentPiecePlacementsStr = piecePlacementsToJsonStr(
        chessRule_->collectPieces(agentColor_));

    std::string opponentPiecePlacementsStr = piecePlacementsToJsonStr(
        chessRule_->collectPieces(chess::getOpponentColor(agentColor_)));

    // TODO: handle the error case when there are no piece placements
    // emit(MoveResponse{ErrorCode{-1, "Mock", "No piece placements found"}});

    ///< send the prompt to the agent
    std::string opponentLastMove = chessMoveDetailToJsonStr(
        chessRule_->getLastMove());
    std::string pastFailureMessages;

    for (int i = 0; i < maxPromptRetries_; ++i)
    {
      std::string input = std::format(
          R"(
          {{
            "yourColor": "{}",
            "pastFailureMessages": [ {} ],
            "yourPiecePlacements": {{ {} }},
            "opponentPiecePlacements": {{ {} }},
            "opponentLastMove": {}
          }})",
          agentColor_.toString(),
          pastFailureMessages,
          agentPiecePlacementsStr,
          opponentPiecePlacementsStr,
          opponentLastMove);

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
      }
      catch (std::exception const &e)
      {
        SPDLOG_WARN(
            "Prompt failure number: {} (JSON parsing failure: {}).\nRetrying...",
            i + 1, e.what());
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        continue;
      }

      ChessMove::Detail agentMoveDetail{};
      try
      {
        agentMoveDetail = chessRule_->tryMove(agentMove);
        // handle the error case when 'agentMove' is invalid
        if (auto invalidMove = agentMoveDetail.getIf<ChessMove::Invalid>())
        {
          throw std::logic_error(invalidMove->errorMessage);
        }
      }
      catch (std::exception const &e)
      {
        // std::string promoveValue = responseMove.promote
        //                                ? "\"" + *(responseMove.promote) + "\""
        //                                : std::string("null");
        std::string failureMsg = std::format("\"{}\"", e.what());

        if (pastFailureMessages.empty())
        {
          pastFailureMessages = failureMsg;
        }
        else
        {
          pastFailureMessages += ",\n" + failureMsg;
        }

        SPDLOG_WARN(
            "Prompt failure number: {} (rule violation: {}).\nRetrying...",
            i + 1,
            pastFailureMessages);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        continue;
      }

      // moveHistoryStr_ += ",\"" + uicResponseMove + "\"";
      chessRule_->commitMove(agentMoveDetail);

      Side currentTurn = chess::getOpponentColor(agentColor_);
      emit(GameUpdatedNotification{agentMoveDetail, currentTurn, currentTurn});

      if (ChessMove::getOpponentKingStatus(agentMoveDetail) ==
          Side::Status::CHECKMATED)
      {
        emit(GameFinishedNotification{"Lose"});
      }
      return;
    }

    emit(GameFinishedNotification{"Error"});
    SPDLOG_ERROR("Completely failed to send prompt to the agent");
    // TODO: handle the failure case after 100 times of try to send prompt to agent
    // msg = e.what();
    // errCodeValue = -1;
    // emit(FailureNotif{ErrorCode{errCodeValue, "Mock", msg}});
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
            "color": {},
            "movedPiece": "{}",
            "fromSquare": "{}",
            "toSquare": "{}",
            "yourCapturedPiece": {},
            "yourKingSafety": "{}"
          }})",
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
            "color": {},
            "movedPiece": "{}",
            "fromSquare": "{}",
            "toSquare": "{}",
            "promotedPiece": "{}",
            "yourCapturedPiece": {},
            "yourKingSafety": "{}"
          }})",
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
            "color": {},
            "movedPiece": "{}",
            "fromSquare": "{}",
            "toSquare": "{}",
            "enPassantCaptureSquare": "{}",
            "yourCapturedPiece": "{}",
            "yourKingSafety": "{}"
          }})",
          enPassantCapture->color.toString(),
          enPassantCapture->movedPiece.toString(),
          enPassantCapture->fromSquare.toString(),
          enPassantCapture->toSquare.toString(),
          enPassantCapture->enPassantCaptureSquare.toString(),
          enPassantCapture->capturedPiece.toString(),
          Side::toString(enPassantCapture->opponentKingStatus));
    }
    else if (auto castling = moveDetail.getIf<ChessMove::Castling>())
    {
      return std::format(
          R"(
          {{
            "type": "Castling",
            "color": {},
            "movedPiece": "{}",
            "fromSquare": "{}",
            "toSquare": "{}",
            "rookSource": "{}",
            "rookDestination": "{}",
            "yourKingSafety": "{}"
          }})",
          castling->color.toString(),
          castling->movedPiece.toString(),
          castling->fromSquare.toString(),
          castling->toSquare.toString(),
          castling->rookSource.toString(),
          castling->rookDestination.toString(),
          Side::toString(castling->opponentKingStatus));
    }
    else if (auto invalidMove = moveDetail.getIf<ChessMove::Invalid>())
    {
      std::string promotedPiece = "null";
      if (invalidMove->promotedPiece)
      {
        promotedPiece = "\"" + invalidMove->promotedPiece->toString() + "\"";
      }
      return std::format(
          R"(
          {{
            "type": "InvalidMove",
            "movedPiece": "{}",
            "fromSquare": "{}",
            "promotedPiece": {},
            "toSquare": "{}",
            "message": "{}"
          }})",
          invalidMove->color.toString(),
          invalidMove->fromSquare.toString(),
          invalidMove->toSquare.toString(),
          promotedPiece,
          invalidMove->errorMessage);
    }
    else // if (auto emptyAction moveDetail.getIf<ChessMove::Invalid>())
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

    return std::string();
  }
} // namespace bgg
