// ChessGameService.cpp

#include <boost/assert.hpp>

#include "ChessGameService.h"
#include "base/Logger.h"
#include "model/chess/ChessUtils.h"

namespace bgg
{
  constexpr const char CHESS_GAME_PROMPT_PATTERN[] =
      "You are a master of chess. You play chess using the UCI (Universal Chess Interface) protocol.\n"
      "Example:\n"
      "- e2e4 means moving a piece from square e2 to e4.\n"
      "- e7e8q means moving a white pawn from e7 to e8 and promoting it to a white queen.\n"
      "- And so on.\n"
      "Notice that you should NEVER use algebraic notation because your opponent may not be able to interpret it..\n"
      "You play as {}.\n"
      "You will receive a sequence of moves in UCI format. Respond concisely with the next move.\n"
      "Example:\n\n"

      "Input: e2e4 \n"
      "Your reply: e7e5\n\n"

      "Input: e2e4 e7e5 g1f3\n"
      "Your reply: d7d6\n\n"

      "Now you receive: {}\n"
      "Let's think step by step...\n"
      "Your reply:";

  ChessGameService::ChessGameService(
      std::string apiKey,
      std::shared_ptr<ClientEventBus> eventBus)
      : threadPool_(2),
        eventBus_(std::move(eventBus)),
        agent_(std::move(apiKey), CHESS_GAME_PROMPT_PATTERN)
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
            emit(MoveResponse{ErrorCode{0, "Mock", ""}});
            SPDLOG_DEBUG("Converted a move request to UIC: {}", uicRequestMove);
          }
          catch (std::exception const &e)
          {
            std::string msg = e.what();
            int errCodeValue = -1;
            emit(MoveResponse{ErrorCode{errCodeValue, "Mock", msg}});
            return;
          }

          // =================================================================

          std::optional<std::pair<std::string, ChessMove>> responseMove;
          int constexpr MAX_TRIES = 10;
          moveHistory_ += uicRequestMove + ' ';
          for (int i = 0; i < MAX_TRIES; ++i)
          {
            std::string response = agent_.sendPromptWithArgs(agentColor_, moveHistory_);
            responseMove = utils::extractChessMove(response);
            if (!responseMove)
              continue;

            auto &[uicResponseMove, chessMoveObj] = *responseMove;
            moveHistory_ += uicResponseMove + ' ';

            std::string yourTurn = Color::WHITE;
            std::string currentTurn = Color::WHITE;
            emit(GameUpdatedNotification{
                chessMoveObj.fromSquare,
                chessMoveObj.toSquare,
                chessMoveObj.promote,
                yourTurn,
                currentTurn});
            SPDLOG_DEBUG("Move history: {}", moveHistory_);
            return;
          }

          // TODO: handle the failure case after 100 times of try to send prompt to agent
          // msg = e.what();
          // errCodeValue = -1;
          // emit(MoveResponse{ErrorCode{errCodeValue, "Mock", msg}});
        });
  }
} // namespace bgg
