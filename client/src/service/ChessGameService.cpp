// ChessGameService.cpp

#include <boost/assert.hpp>

#include "ChessGameService.h"
#include "base/Logger.h"
#include "model/chess/ChessUtils.h"

namespace bgg
{
  constexpr const char CHESS_GAME_PROMPT_PATTERN[] = "Hi there!";

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
          int constexpr MAX_TRIES = 5;
          for (int i = 0; i < MAX_TRIES; ++i)
          {
            moveHistory_ += '`' + uicRequestMove + '`';
            std::string response = agent_.sendPromptWithArgs(agentColor_, moveHistory_);
            responseMove = utils::extractChessMove(response);
            if (!responseMove)
              continue;

            auto &[uicResponseMove, chessMoveObj] = *responseMove;
            moveHistory_ += '`' + uicResponseMove + '`';

            std::string yourTurn = Color::WHITE;
            std::string currentTurn = Color::WHITE;
            emit(GameUpdatedNotification{
                chessMoveObj.fromSquare,
                chessMoveObj.toSquare,
                chessMoveObj.promote,
                yourTurn,
                currentTurn});
            return;
          }

          // TODO: handle the failure case after 100 times of try to send prompt to agent
          // msg = e.what();
          // errCodeValue = -1;
          // emit(MoveResponse{ErrorCode{errCodeValue, "Mock", msg}});
        });
  }
} // namespace bgg
