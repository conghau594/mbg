// SfGameFindingScreen.cpp

#include "SfGameFindingScreen.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include "base/Logger.h"

#include "GameDisplay.h"
#include "SfMessageScreen.h"
#include "SfGamePlayScreen.h"

#include "service/ClientRequest.h"
#include "service/ServerMessage.h"

#include "display/board/SfGameBoardFactory.h"

namespace bgg
{
  SfGameFindingScreen::SfGameFindingScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameDisplay> gameDisplay,
      int gameType,
      int playerType)
      : SfMessageScreen(
            std::move(window),
            "Waiting for oppenent...",
            {"Cancel"},
            {[this]
             { sendCancelMatchmakingRequest(); }}),
        gameDisplay_(std::move(gameDisplay)),
        gameType_(gameType),
        playerType_(playerType),
        isCancelButtonPressed_(false)
  {
    getServerMsgHandler().setHandler<FindGameResponse>(
        [this](FindGameResponse const &response) -> bool
        {
          onFindGameResponse(response);
          return true;
        });

    getServerMsgHandler().setHandler<CancelMatchmakingResponse>(
        [this](CancelMatchmakingResponse const &response) -> bool
        {
          onCancelMatchmakingResponse(response);
          return true;
        });
  }

  void SfGameFindingScreen::update(sf::Time const &elapsed) noexcept
  {

    SfMessageScreen::update(elapsed);
  }

  void SfGameFindingScreen::goToGamePlayScreen()
  {
    try
    {
      // TODO: Get value of `side` from GameStartedNotif from server
      int side = 0;
      //==============
      int resignRegionHeight = 60;
      sf::Vector2i wndSize = sf::Vector2i(getWindow()->getSize());
      std::shared_ptr<SfGameBoard> chessBoard = SfGameBoardFactory().create(
          side, gameType_, sf::IntRect({0, 0}, wndSize));
      std::shared_ptr<GameScreen> chessScreen = std::make_shared<SfGamePlayScreen>(
          getWindow(), gameDisplay_, chessBoard, resignRegionHeight);

      gameDisplay_->changeScreen(chessScreen);
    }
    catch (std::exception const &e)
    {
      // TODO: need to handle this exception in detail
      spdlog::critical("From SfGameFindingScreen::goToGamePlayScreen: {}", e.what());
    }
  }

  void SfGameFindingScreen::sendCancelMatchmakingRequest() noexcept
  {
    // TODO: sendCancelMatchmakingRequest()
    // gameDisplay_->send(CancelMatchmakingRequest{});

    // std::shared_ptr<GameScreen> cancelingScreen = std::make_shared<SfMessageScreen>(
    //     getWindow(), "Canceling...");

    // changeSubscreen(cancelingScreen);
  }

  void SfGameFindingScreen::onCancelMatchmakingResponse(CancelMatchmakingResponse const & /*response*/) noexcept
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

  void SfGameFindingScreen::onFindGameResponse(FindGameResponse const &response) noexcept
  {
    // TODO: this is just a simple implementation without synchronization with
    //       CancelMatchmakingRequest -> need to reimplement
    ErrorCode const &errcode = response.errcode;
    if (errcode.value == 0) // no error -> the game is found
    {
      goToGamePlayScreen();
      return;
    }
    else
    {
      std::string message = errcode.message + " (" +
                            std::to_string(errcode.value) + ")";
      auto okButtonCallback = [this]
      {
        changeSubscreen(nullptr);
        gameDisplay_->popScreen();
      };

      std::shared_ptr<GameScreen> messageScreen = std::make_shared<SfMessageScreen>(
          getWindow(),
          message,
          std::vector<std::string>{"OK"},
          std::vector<std::function<void()>>{okButtonCallback});

      changeSubscreen(messageScreen);
      return;
    }
  }
} // namespace bgg
