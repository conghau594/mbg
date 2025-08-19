// GameFindingScreen.cpp

#include "GameFindingScreen.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include "base/Logger.h"

#include "display/IDisplay.h"
#include "MessageScreen.h"
#include "GamePlayScreen.h"

#include "service/ClientRequest.h"
#include "service/ServerMessage.h"
#include "model/chess/ChessRule.h"

#include "display/board/GameBoardFactory.h"

namespace bgg
{
  GameFindingScreen::GameFindingScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<IDisplay> gameDisplay)
      : MessageScreen(
            std::move(window),
            "Waiting for oppenent...",
            {"Cancel"},
            {[this]
             { sendCancelMatchmakingRequest(); }}),
        gameDisplay_(std::move(gameDisplay)),
        isCancelButtonPressed_(false)
  {
  }

  void GameFindingScreen::update(sf::Time const &elapsed) noexcept
  {
    MessageScreen::update(elapsed);
  }

  void GameFindingScreen::doEnter()
  {
    MessageScreen::doEnter();

    getServerMsgHandler().setHandler<FindGameResponse>(
        [this](FindGameResponse const &response) -> bool
        {
          onFindGameResponse(response);
          SPDLOG_INFO("A message of type '{}' has been handled by '{}'",
                      typeid(response).name(), typeid(*this).name());
          return true;
        });

    getServerMsgHandler().setHandler<CancelMatchmakingResponse>(
        [this](CancelMatchmakingResponse const &response) -> bool
        {
          onCancelMatchmakingResponse(response);
          SPDLOG_INFO("A message of type '{}' has been handled by '{}'",
                      typeid(response).name(), typeid(*this).name());
          return true;
        });
  }

  void GameFindingScreen::doExit()
  {
    MessageScreen::doExit();
    getServerMsgHandler().resetHandler<FindGameResponse>();
    getServerMsgHandler().resetHandler<CancelMatchmakingResponse>();
  }

  void GameFindingScreen::sendCancelMatchmakingRequest() noexcept
  {
    // TODO: sendCancelMatchmakingRequest()
    // gameDisplay_->send(CancelMatchmakingRequest{});

    // std::shared_ptr<IScreen> cancelingScreen = std::make_shared<MessageScreen>(
    //     getWindow(), "Canceling...");

    // changeSubscreen(cancelingScreen);
  }

  void GameFindingScreen::onCancelMatchmakingResponse(
      CancelMatchmakingResponse const & /*response*/) noexcept
  {
    // TODO: onCancelMatchmakingResponse(CancelMatchmakingResponse)
    //  ErrorCode const &errcode = response.errcode;
    //  if (errcode.value == 0) // no error -> canceled successfully
    //  {
    //    gameDisplay_->popScreen();

    //   return;
    // }
    // else
    // {
    //   std::string message = "Failed to cancel matchmaking\n(" +
    //                         errcode.message + " (" +
    //                         std::to_string(errcode.value) + "))";
    //   // break;
    // }
  }

  void GameFindingScreen::onFindGameResponse(
      FindGameResponse const &response) noexcept
  {
    // TODO: this is just a simple implementation without synchronization with
    //       CancelMatchmakingRequest -> you need to reimplement along with
    //       the function onCancelMatchmakingResponse.
    ErrorCode const &errcode = response.errcode;
    if (errcode.value == 0) // no error -> the game is found
    {
      if (response.gameType == GameType::CHESS)
      {

        int constexpr RESIGN_REGION_HEIGHT = 80;
        int constexpr BOARD_SIDE_LENGTH = 800;
        getWindow()->setSize(
            {BOARD_SIDE_LENGTH, BOARD_SIDE_LENGTH + RESIGN_REGION_HEIGHT});

        sf::IntRect boardRect(
            {0, RESIGN_REGION_HEIGHT}, {BOARD_SIDE_LENGTH, BOARD_SIDE_LENGTH});
        std::shared_ptr<IChessRule>
            chessRule = std::make_shared<ChessRule>(response.initialPlacements);

        std::weak_ptr<IDisplay> gameDisplayWeakPtr(gameDisplay_);
        auto requestSender = [gameDisplayWeakPtr](ClientRequest const &request) noexcept
        {
          if (auto gameDisplayPtr = gameDisplayWeakPtr.lock())
          {
            gameDisplayPtr->send(request);
          }
        };

        std::shared_ptr<IBoardView> chessBoard = GameBoardFactory().createChessBoard(
            std::move(chessRule),
            boardRect,
            std::move(requestSender),
            response.yourSide,
            response.yourSide == response.currentTurn);

        std::shared_ptr<IScreen> chessScreen = std::make_shared<GamePlayScreen>(
            getWindow(), gameDisplay_, chessBoard, RESIGN_REGION_HEIGHT);
        changeSubscreen(nullptr);
        gameDisplay_->changeScreen(chessScreen);

        return;
      }
      else
      {
        SPDLOG_ERROR("Unsupported game type: {}", response.gameType);
        return;
      }
    }
    else
    {
      auto message = std::format("{} ({})", errcode.message, errcode.value);
      auto okButtonCallback = [this]
      {
        changeSubscreen(nullptr);
        gameDisplay_->popScreen();
      };

      std::shared_ptr<IScreenInternal> messageScreen = std::make_shared<MessageScreen>(
          getWindow(),
          message,
          std::vector<std::string>{"OK"},
          std::vector<std::function<void()>>{okButtonCallback});

      changeSubscreen(messageScreen);
      return;
    }
  }
} // namespace bgg
