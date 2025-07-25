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
          "yourColor": "Black"

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
        For examples:
          1. "opponentLastMove": {
              "type": "Normal",
              "fromSquare": "e7",
              "toSquare": "e6",
              "yourKingSafety": "Safe"
            }
          2. "opponentLastMove": {
              "type": "Promotion",
              "fromSquare": "e7",
              "toSquare": "f8",
              "promote": "Queen",
              "yourKingSafety": "In check"
            }
          3. "opponentLastMove": {
              "type": "EnPassant",
              "fromSquare": "e5",
              "toSquare": "d6",
              "enPassantCaptureSquare": "d5",
              "yourKingSafety": "Safe"
            }
          4. "opponentLastMove": {
              "type": "Castling",
              "fromSquare": "e8",
              "fromSquare": "g8",
              "rookSource": "h8",
              "rookDestination": "f8",
              "yourKingSafety": "In check"
            }
        A null opponentLastMove means it's your turn to play first.

        Using this information, figure out the current board state, understand 
        your opponent's intentions through its last move, and respond 
        with your next move in UCI (Universal Chess Interface) format.
        For example: 
          {
            "purpose": "<brief subjective reason why you do this move (at most 3 sentences)>",
            "fromSquare": "e7",
            "toSquare": "e6",
            "promote": null
          }

        Sometimes, you make an invalid move. You can refer to "pastFailures" 
        and take it as a lesson then try again.
        For example:
          "pastFailures": [
            {
              "wrongMove": {
                "fromSquare": "d8",
                "toSquare": "d6"
              },
              "message": "Invalid destination"
            },
            {
              "wrongMove": {
                "fromSquare": "d8",
                "toSquare": "d6",
                "promote": "Queen"
              },
              "message": "Invalid promotion"
            },
            {
              "wrongMove": {
                "fromSquare": "h1",
                "toSquare": "h8"
              },
              "message": "Your king is in check"
            },
            ...
          ]

        Make sure to:
        - Always evaluate the safety of your king before making any move.
        - Ensure your move is valid and keeps your king out of check.

        <EXAMPLE>
          INPUT: 
            {
              "yourColor": "Black",
              "pastFailures": [],
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
              "pastFailures": [
                {
                  "wrongMove": "d6",
                  "message": "Invalid UCI move format"
                }
              ],
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
              "pastFailures": [],
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
      std::shared_ptr<ChessBoardState> chessRule,
      std::shared_ptr<ClientEventBus> eventBus)
      : threadPool_(2),
        eventBus_(std::move(eventBus)),
        subscriptionIdList_{},
        agent_(std::move(apiKey),
               CHESS_GAME_SYSTEM_INSTRUCTION,
               CHESS_MOVE_JSON_SCHEMA,
               CHESS_GAME_PROMPT_PATTERN),
        chessRule_(std::move(chessRule)),
        agentColor_(ChessRule::getEnemyColor(chessRule_->getAllyColor()))
  // moveHistoryStr_{}
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
          ///< Validate the move request
          ChessMove::Action yourMoveAction;
          try
          {
            yourMoveAction = chessRule_->tryMove(
                ChessMove{moveRqt.fromSquare, moveRqt.toSquare, moveRqt.promote},
                chessRule_->getAllyColor());

            if (yourMoveAction.isEmpty() ||
                yourMoveAction.is<ChessMove::Invalid>())
            {
              throw std::invalid_argument(std::format(
                  "Invalid move request: from {}{} to {}{} for color {}",
                  moveRqt.fromSquare[0], moveRqt.fromSquare[1],
                  moveRqt.toSquare[0], moveRqt.toSquare[1],
                  chessRule_->getAllyColor()));
            }

            emit(MoveResponse{ErrorCode{0, "Mock", ""}});
            chessRule_->commitMove(yourMoveAction);
          }
          catch (std::exception const &e)
          {
            int errCodeValue = -1;
            emit(MoveResponse{ErrorCode{errCodeValue, "Mock", e.what()}});
            return;
          }

          sendChessGamePromptToAgent();
        });
  }

  void ChessGameService::sendChessGamePromptToAgent()
  {
    ///< get current placements
    auto const &piecePlacements = chessRule_->getPiecePlacements();
    if (piecePlacements.empty())
    {
      SPDLOG_ERROR("No piece placements found in the chess rule");
      // TODO: handle the error case when there are no piece placements
      // emit(MoveResponse{ErrorCode{-1, "Mock", "No piece placements found"}});
      return;
    }
    std::string currentPlacements;
    for (auto &[square, piece] : piecePlacements)
    {
      currentPlacements += std::format(
          R"("{}{}":"{}",)", square[0], square[1], piece.toString());
    }
    currentPlacements.pop_back(); // remove the last comma

    ///< send the prompt to the agent
    int constexpr MAX_TRIES = 10;
    std::string pastFailures;
    std::string opponentLastMove = chessMoveActionToJsonString(
        chessRule_->getLastMoveAction());
    for (int i = 0; i < MAX_TRIES; ++i)
    {
      std::string input = std::format(
          R"({{
            "yourColor": "{}",
            "pastFailures": [ {} ],
            "currentPlacements": {{ {} }},
            "opponentLastMove": {}
          }})",
          agentColor_, pastFailures, currentPlacements, opponentLastMove);

      SPDLOG_DEBUG("Sending prompt to agent: {}", input);

      std::optional<std::string> response = agent_.sendPromptWithArgs(input);
      if (!response)
      {
        SPDLOG_WARN(
            "Prompt failure number: {} (connection error). Retrying...",
            i + 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        continue;
      }

      ChessMove responseMove;
      std::string uicResponseMove;
      try
      {
        responseMove = utils::jsonToChessMoveObject(*response);
        // uicResponseMove = utils::chessMoveObjectToUic(responseMove);

        ChessMove::Action moveAction =
            chessRule_->tryMove(responseMove, agentColor_);
        // handle the error case when 'responseMove' is invalid
        if (auto invalidAction = moveAction.getIf<ChessMove::Invalid>())
        {
          throw std::runtime_error(utils::toString(invalidAction->error));
        }

        std::string currentTurn = Color::WHITE;
        emit(GameUpdatedNotification{
            responseMove.fromSquare,
            responseMove.toSquare,
            responseMove.promote,
            chessRule_->getAllyColor(),
            currentTurn});

        // moveHistoryStr_ += ",\"" + uicResponseMove + "\"";
        chessRule_->commitMove(moveAction);

        return;
      }
      catch (std::exception const &e)
      {
        std::string promoveValue = responseMove.promote
                                       ? "\"" + *(responseMove.promote) + "\""
                                       : std::string("null");
        std::string failure = std::format(
            R"({{
                "wrongMove": {{
                  "fromSquare": "{}{}",
                  "toSquare": "{}{}",
                  "promote": {}
                }}, 
                "message": "{}"
            }})",
            responseMove.fromSquare[0], responseMove.fromSquare[1],
            responseMove.toSquare[0], responseMove.toSquare[1],
            promoveValue,
            e.what());

        if (pastFailures.empty())
        {
          pastFailures = failure;
        }
        else
        {
          pastFailures += ", " + failure;
        }

        SPDLOG_WARN(
            "Prompt failure number: {} (logic error: {}). Retrying...",
            i + 1,
            pastFailures);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        continue;
      }
    }

    SPDLOG_ERROR("Prompt request completely failed");
    // TODO: handle the failure case after 100 times of try to send prompt to agent
    // msg = e.what();
    // errCodeValue = -1;
    // emit(FailureNotif{ErrorCode{errCodeValue, "Mock", msg}});
  }

  auto ChessGameService::chessMoveActionToJsonString(
      ChessMove::Action const &moveAction) noexcept -> std::string
  {
    if (auto normalAction = moveAction.getIf<ChessMove::Normal>())
    {
      return std::format(
          R"(
          {{
            "type": "Normal",
            "fromSquare": "{}{}",
            "toSquare": "{}{}",
            "yourKingSafety": "{}"
          }})",
          normalAction->fromSquare[0], normalAction->fromSquare[1],
          normalAction->toSquare[0], normalAction->toSquare[1],
          utils::toString(normalAction->enemyKingState));
    }
    else if (auto promotion = moveAction.getIf<ChessMove::Promotion>())
    {
      return std::format(
          R"(
          {{
            "type": "Promotion",
            "fromSquare": "{}{}",
            "toSquare": "{}{}",
            "promote": "{}",
            "yourKingSafety": "{}"
          }})",
          promotion->fromSquare[0], promotion->fromSquare[1],
          promotion->toSquare[0], promotion->toSquare[1],
          promotion->promote,
          utils::toString(promotion->enemyKingState));
    }
    else if (auto enPassantCapture = moveAction.getIf<ChessMove::EnPassant>())
    {
      return std::format(
          R"(
          {{
            "type": "EnPassant",
            "fromSquare": "{}{}",
            "toSquare": "{}{}",
            "enPassantCaptureSquare": "{}{}",
            "yourKingSafety": "{}"
          }})",
          enPassantCapture->fromSquare[0], enPassantCapture->fromSquare[1],
          enPassantCapture->toSquare[0], enPassantCapture->toSquare[1],
          enPassantCapture->enPassantSquare[0], enPassantCapture->enPassantSquare[1],
          utils::toString(enPassantCapture->enemyKingState));
    }
    else if (auto castling = moveAction.getIf<ChessMove::Castling>())
    {
      return std::format(
          R"(
          {{
            "type": "Castling",
            "fromSquare": "{}{}",
            "toSquare": "{}{}",
            "rookSource": "{}{}",
            "rookDestination": "{}{}",
            "yourKingSafety": "{}"
          }})",
          castling->fromSquare[0], castling->fromSquare[1],
          castling->toSquare[0], castling->toSquare[1],
          castling->rookSource[0], castling->rookSource[1],
          castling->rookDestination[0], castling->rookDestination[1],
          utils::toString(castling->enemyKingState));
    }
    else if (auto invalidAction = moveAction.getIf<ChessMove::Invalid>())
    {
      return std::format(
          R"(
          {{
            "type": "Invalid",
            "code": "{}",
            "message": "{}"
          }})",
          int(invalidAction->error),
          utils::toString(invalidAction->error));
    }
    else if (auto emptyAction = moveAction.getIf<ChessMove::Invalid>())
    {
      return "null";
    }
  }
} // namespace bgg
