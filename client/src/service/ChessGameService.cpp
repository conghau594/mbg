// ChessGameService.cpp

#include <boost/assert.hpp>

#include "ChessGameService.h"
#include "base/Logger.h"
#include "model/chess/ChessUtils.h"
#include "model/chess/ChessBoardState.h"

namespace bgg
{
  constexpr const char CHESS_GAME_SYSTEM_INSTRUCTION[] =
      "You are a master of chess.\n"
      "You play using the UCI (Universal Chess Interface) protocol.\n"
      "For example:\n"
      "- \"e2e4\" means moving a piece from square e2 to square e4.\n"
      "- \"e7e8q\" means promoting a pawn from e7 to e8 into a queen.\n"
      "- And so on.\n"
      "\n"
      "You will be given the color of the pieces you are controlling.\n"
      "For example:\n"
      "  COLOR: Black\n"
      "\n"
      "You will then receive the move history of the game in UCI format.\n"
      "For example:\n"
      "  MOVE_HISTORY: e2e4 e7e5 g1f3 ...\n"
      "This means that White played e2 to e4 on the first move, Black responded with e7 to e5,\n"
      "White followed with g1 to f3, and so on.\n"
      "An empty move history means it's your turn to play first.\n"
      "\n"
      "You will also receive the current placement of all pieces on the board.\n"
      "For example:\n"
      "  CURRENT_PLACEMENTS: a1=WhiteRook b1=WhiteKnight ...\n"
      "This indicates that square a1 contains a White Rook, b1 contains a White Knight, and so on.\n"
      "\n"
      "Using this information, reconstruct the current board state, understand your opponent's\n"
      "intentions through the move history, and respond concisely with your next move in UCI format.\n"
      "For example:\n"
      "  NEXT_UCI_MOVE: e7e6"
      "\n"
      "Make sure to:\n"
      "- Always evaluate the safety of your king before making any move.\n"
      "- Ensure your move is valid and keeps your king out of check.\n"
      "- Your final output must be in UCI format (not algebraic notation).\n"
      "- You should not include any explanations or commentary in your output.\n"
      "Sometimes, you forget to convert your move to UCI format or make an invalid move.\n"
      "You can refer to LAST_FAILURE to retry that move.\n"
      "\n"
      "<EXAMPLE>\n"
      "  COLOR: Black\n"
      "  MOVE_HISTORY: e2e4\n"
      "  CURRENT_PLACEMENTS:\n"
      "    a1=WhiteRook b1=WhiteKnight c1=WhiteBishop d1=WhiteQueen e1=WhiteKing f1=WhiteBishop g1=WhiteKnight h1=WhiteRook\n"
      "    a2=WhitePawn b2=WhitePawn c2=WhitePawn d2=WhitePawn e4=WhitePawn f2=WhitePawn g2=WhitePawn h2=WhitePawn\n"
      "    a8=BlackRook b8=BlackKnight c8=BlackBishop d8=BlackQueen e8=BlackKing f8=BlackBishop g8=BlackKnight h8=BlackRook\n"
      "    a7=BlackPawn b7=BlackPawn c7=BlackPawn d7=BlackPawn e7=BlackPawn f7=BlackPawn g7=BlackPawn h7=BlackPawn\n"
      "  LAST_FAILURE: \n"
      "  NEXT_UCI_MOVE: e7e5\n"
      "</EXAMPLE>\n"
      "\n"
      "<EXAMPLE>\n"
      "  COLOR: Black\n"
      "  MOVE_HISTORY: e2e4 e7e5 g1f3\n"
      "  CURRENT_PLACEMENTS: \n"
      "    a1=WhiteRook b1=WhiteKnight c1=WhiteBishop d1=WhiteQueen e1=WhiteKing f1=WhiteBishop f3=WhiteKnight h1=WhiteRook\n"
      "    a2=WhitePawn b2=WhitePawn c2=WhitePawn d2=WhitePawn e4=WhitePawn f2=WhitePawn g2=WhitePawn h2=WhitePawn\n"
      "    a8=BlackRook b8=BlackKnight c8=BlackBishop d8=BlackQueen e8=BlackKing f8=BlackBishop g8=BlackKnight h8=BlackRook\n"
      "    a7=BlackPawn b7=BlackPawn c7=BlackPawn d7=BlackPawn e5=BlackPawn f7=BlackPawn g7=BlackPawn h7=BlackPawn\n"
      "  LAST_FAILURE: d6\n"
      "  NEXT_UCI_MOVE: d7d6\n"
      "</EXAMPLE>\n"
      "\n"
      "<EXAMPLE>\n"
      "  COLOR: White\n"
      "  MOVE_HISTORY: \n"
      "  CURRENT_PLACEMENTS: \n"
      "    a1=WhiteRook b1=WhiteKnight c1=WhiteBishop d1=WhiteQueen e1=WhiteKing f1=WhiteBishop g1=WhiteKnight h1=WhiteRook\n"
      "    a2=WhitePawn b2=WhitePawn c2=WhitePawn d2=WhitePawn e4=WhitePawn f2=WhitePawn g2=WhitePawn h2=WhitePawn\n"
      "    a8=BlackRook b8=BlackKnight c8=BlackBishop d8=BlackQueen e8=BlackKing f8=BlackBishop g8=BlackKnight h8=BlackRook\n"
      "    a7=BlackPawn b7=BlackPawn c7=BlackPawn d7=BlackPawn e7=BlackPawn f7=BlackPawn g7=BlackPawn h7=BlackPawn\n"
      "  NEXT_UCI_MOVE: d2d4\n"
      "</EXAMPLE>\n";

  constexpr const char CHESS_GAME_PROMPT_PATTERN[] =
      "Let's think step by step:\n"
      "YOUR_COLOR: {}\n"
      "BOARD_HISTORY: {}\n"
      "CURRENT_PLACEMENTS: {}\n"
      "LAST_FAILURE: {}\n"
      "NEXT_UCI_MOVE: ";

  ChessGameService::ChessGameService(
      std::string apiKey,
      std::shared_ptr<ChessBoardState> chessRule,
      std::shared_ptr<ClientEventBus> eventBus)
      : threadPool_(2),
        eventBus_(std::move(eventBus)),
        subscriptionIdList_{},
        agent_(std::move(apiKey),
               CHESS_GAME_SYSTEM_INSTRUCTION,
               CHESS_GAME_PROMPT_PATTERN),
        chessRule_(std::move(chessRule)),
        agentColor_(ChessRule::getEnemyColor(chessRule_->getAllyColor())),
        moveHistoryStr_{}
  {
    auto subscriptionId = eventBus_->subscribe<ClientRequest, MoveRequest>(
        [this](MoveRequest const &moveRqt)
        {
          handleMoveRequest(moveRqt);
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
    for (auto &id : subscriptionIdList_)
    {
      eventBus_->unsubscribe<ClientRequest>(id);
    }
  }

  void ChessGameService::emit(ServerMessage const &msg) noexcept
  {
    eventBus_->emit<ServerMessage>(msg);
  }

  void ChessGameService::handleMoveRequest(
      MoveRequest const &moveRqt) noexcept
  {
    threadPool_.push(
        [this, moveRqt]()
        {
          std::string uicRequestMove;
          try
          {
            uicRequestMove = ChessRule::toUic(ChessMove{
                moveRqt.fromSquare, moveRqt.toSquare, moveRqt.promote});

            ChessMove::Action moveAction = chessRule_->tryMove(
                ChessMove{moveRqt.fromSquare, moveRqt.toSquare, moveRqt.promote},
                chessRule_->getAllyColor());

            if (moveAction.isEmpty() || moveAction.is<ChessMove::Invalid>())
            {
              throw std::invalid_argument(std::format(
                  "Invalid move request: {}{} to {}{} for color {}",
                  moveRqt.fromSquare[0], moveRqt.fromSquare[1],
                  moveRqt.toSquare[0], moveRqt.toSquare[1],
                  chessRule_->getAllyColor()));
            }

            emit(MoveResponse{ErrorCode{0, "Mock", ""}});

            chessRule_->commitMove(moveAction);
            // SPDLOG_DEBUG("Converted a move request to UIC: {}", uicRequestMove);
          }
          catch (std::exception const &e)
          {
            int errCodeValue = -1;
            emit(MoveResponse{ErrorCode{errCodeValue, "Mock", e.what()}});
            return;
          }

          // =================================================================

          std::optional<std::pair<std::string, ChessMove>> responseMove;
          moveHistoryStr_ += uicRequestMove + ' ';
          std::string lastFailure;

          // get current placements
          std::string currentPlacements;
          auto const &piecePlacements = chessRule_->getPiecePlacements();
          for (auto &[square, piece] : piecePlacements)
          {
            currentPlacements += std::format(
                "{}{}={} ", square[0], square[1], piece.toString());
          }
          // SPDLOG_DEBUG("Current placements: {}", currentPlacements);

          int constexpr MAX_TRIES = 10;
          for (int i = 0; i < MAX_TRIES; ++i)
          {

            std::optional<std::string> response = agent_.sendPromptWithArgs(
                agentColor_, moveHistoryStr_, currentPlacements, lastFailure);
            if (!response)
            {
              SPDLOG_WARN("Prompt request failure times: {}. Retrying...", i + 1);
              std::this_thread::sleep_for(std::chrono::milliseconds(1500));
              continue;
            }

            std::regex pattern("NEXT_UCI_MOVE"); // tạo regex từ chuỗi cần xoá
            *response = std::regex_replace(*response, pattern, "");
            responseMove = utils::extractChessMove(*response);
            if (!responseMove)
            {
              lastFailure += (*response) + ", ";
              SPDLOG_WARN("Prompt request failure times: {}. Retrying...", i + 1);
              std::this_thread::sleep_for(std::chrono::milliseconds(500));
              continue;
            }

            auto &[uicResponseMove, chessMoveObj] = *responseMove;
            ChessMove::Action moveAction =
                chessRule_->tryMove(chessMoveObj, agentColor_);
            // handle the error case when chessMoveObj is invalid
            if (moveAction.isEmpty() || moveAction.is<ChessMove::Invalid>())
            {
              lastFailure += uicResponseMove + ", ";
              SPDLOG_WARN("Prompt request failure times: {}. Retrying...", i + 1);
              std::this_thread::sleep_for(std::chrono::milliseconds(500));
              continue;
            }

            moveHistoryStr_ += uicResponseMove + ' ';

            std::string yourTurn = Color::WHITE;
            std::string currentTurn = Color::WHITE;
            emit(GameUpdatedNotification{
                chessMoveObj.fromSquare,
                chessMoveObj.toSquare,
                chessMoveObj.promote,
                yourTurn,
                currentTurn});
            SPDLOG_DEBUG("Move history: {}", moveHistoryStr_);

            chessRule_->commitMove(moveAction);
            ///< reset lastFailure
            lastFailure = "";
            return;
          }

          SPDLOG_ERROR("Prompt request completely failed");

          // TODO: handle the failure case after 100 times of try to send prompt to agent
          // msg = e.what();
          // errCodeValue = -1;
          // emit(MoveResponse{ErrorCode{errCodeValue, "Mock", msg}});
        });
  }
} // namespace bgg
