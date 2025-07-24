// ChessGameService.cpp

#include <boost/assert.hpp>

#include "ChessGameService.h"
#include "base/Logger.h"
#include "model/chess/ChessUtils.h"
#include "model/chess/ChessBoardState.h"

namespace bgg
{
  constexpr const char CHESS_GAME_SYSTEM_INSTRUCTION[] =
      R"(
      You are a master of chess. You play using the UCI  protocol.
      You will be given the color of the pieces you are controlling.
      For example:
        "color": "Black"

      You will then receive the current placement of all pieces on the board.
      For example:
        "currentPlacements": {
          "a1": "WhiteRook",
          "b1": "WhiteKnight",
           ...
        }
      This indicates that square a1 contains a White Rook, b1 contains a 
      White Knight, and so on.

      You will also receive the move history of the game in UCI format.
      For example:
        "moveHistory": [ "e2e4", "e7e5", "g1f3", ... , "e7e8q", ... ]
      This means that White played e2 to e4 on the first move, Black responded 
      with e7 to e5, White followed with g1 to f3, and so on. 
      Notice that, e7e8q means moving a white pawn from e7 to e8 and promoting 
      it to a white queen.
      An empty move history means it's your turn to play first.

      Using this information, reconstruct the current board state, understand 
      your opponent's intentions through the move history, and respond 
      with your next move in UCI (Universal Chess Interface) format.
      For example: 
        {
          "sourceSquare": "e7",
          "destinationSquare": "e6",
          "promote": "None",
          "reason": "<brief reason why you do this move (at most 3 sentences)>"
        }

      Sometimes, you forget to convert your move to UCI format or make an 
      invalid move. You can refer to "pastFailures" to retry that move correctly.
      For example:
        "pastFailures": [
          {
            "wrongMove": "d8d6",
            "message": "Invalid destination"
          },
          {
            "wrongMove": "e5xd4",
            "message": "Invalid UCI move format"
          },
          {
            "wrongMove": "e2e1k",
            "message": "Invalid promotion"
          },
          {
            "wrongMove": "h1h8",
            "message": "Your king is exposed"
          }
        ]

      Make sure to:
      - Always evaluate the safety of your king before making any move.
      - Ensure your move is valid and keeps your king out of check.
      - Your final output must be in UCI format (not algebraic notation).

    <EXAMPLE>
      INPUT: 
        {
          "color": "Black",
          "currentPlacments": {
            "a1": "WhiteRook", "b1": "WhiteKnight", "c1": "WhiteBishop", "d1": "WhiteQueen",
            "e1": "WhiteKing", "f1": "WhiteBishop", "g1": "WhiteKnight", "h1": "WhiteRook",
            "a2": "WhitePawn", "b2": "WhitePawn", "c2": "WhitePawn", "d2": "WhitePawn",
            "e4": "WhitePawn", "f2": "WhitePawn", "g2": "WhitePawn", "h2": "WhitePawn",
            "a8": "BlackRook", "b8": "BlackKnight", "c8": "BlackBishop", "d8": "BlackQueen",
            "e8": "BlackKing", "f8": "BlackBishop", "g8": "BlackKnight", "h8": "BlackRook",
            "a7": "BlackPawn", "b7": "BlackPawn", "c7": "BlackPawn", "d7": "BlackPawn",
            "e7": "BlackPawn", "f7": "BlackPawn", "g7": "BlackPawn", "h7": "BlackPawn"
          },
          "moveHistory": [ "e2e4" ],
          "pastFailures": []
        } 
      OUTPUT:
        {
          "sourceSquare": "e7",
          "destinationSquare": "e5",
          "promote": "None",
          "reason": "Contesting the center by mirroring White's e4 move. 
                     This is a classical and strong response that opens lines 
                     for the queen and bishop."
        }
    </EXAMPLE>

    <EXAMPLE>
      INPUT: 
        {
          "color": "Black",
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
          "moveHistory": ["e2e4", "e7e5", "g1f3"],
          "pastFailures": [
            {
            "wrongMove": "d6",
            "message": "Invalid UCI move format"
            }
          ]
        }
      OUTPUT:
        {
          "sourceSquare": "b8",
          "destinationSquare": "c6",
          "promote": "None",
          "reason": "Developing the knight to a natural square, controlling 
                     the center (d4 and e5), and preparing for kingside castling."
        }
    </EXAMPLE>

    <EXAMPLE>
      INPUT:
        {  
          "color": "White",
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
          "moveHistory": [],
          "pastFailures": []
        }
      OUTPUT:
        {
          "sourceSquare": "e2",
          "destinationSquare": "e4",
          "promote": "None",
          "reason": "Opening with the King's Pawn controls the center and 
                     opens lines for the queen and bishop. It's a classical 
                     and strong first move."
        }
    </EXAMPLE>
)";

  constexpr const char CHESS_GAME_PROMPT_PATTERN[] =
      R"(
        Let's think step by step:
        {} 
      )";

  constexpr const char CHESS_GAME_RESPONSE_SCHEMA[] =
      R"(
      {
        "type": "OBJECT",
        "properties": {
          "sourceSquare": {
            "type": "STRING",
            "pattern": "^[a-h][1-8]$"
          },
          "destinationSquare": {
            "type": "STRING",
            "pattern": "^[a-h][1-8]$"
          },
          "promote": {
            "type": "STRING",
            "enum": ["None", "Queen", "Rook", "Bishop", "Knight"]
          },
          "reason": { "type": "STRING" }
        }
      }
    )";

  ChessGameService::ChessGameService(
      std::string apiKey,
      std::shared_ptr<ChessBoardState> chessRule,
      std::shared_ptr<ClientEventBus> eventBus)
      : threadPool_(2),
        eventBus_(std::move(eventBus)),
        subscriptionIdList_{},
        agent_(std::move(apiKey),
               CHESS_GAME_SYSTEM_INSTRUCTION,
               JSON_SCHEMA,
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
                  "Invalid move request: from {}{} to {}{} for color {}",
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
                agentColor_, currentPlacements, moveHistoryStr_, lastFailure);
            if (!response)
            {
              SPDLOG_WARN("Prompt request failure times: {}. Retrying...", i + 1);
              std::this_thread::sleep_for(std::chrono::milliseconds(1500));
              continue;
            }

            std::regex pattern("YOUR_UCI_MOVE[:\\s]*"); // tạo regex từ chuỗi cần xoá
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
