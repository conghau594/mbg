// SfGameFindingScreen.cpp

#include "SfGameFindingScreen.h"

#include "SfMessageScreen.h"
#include "GameDisplay.h"

#include "service/ClientEvent.h"
#include "service/ServerMessage.h"

#include "SfChessScreen.h"

namespace bgg
{
  SfGameFindingScreen::SfGameFindingScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameDisplay> gameDisplay,
      int gameType,
      int playerType)
      : gameDisplay_(std::move(gameDisplay)),
        gameType_(gameType),
        playerType_(playerType),
        isCancelButtonPressed_(false),
        SfMessageScreen(
            std::move(window),
            "Waiting for oppenent...",
            {"Cancel"},
            {[this]
             { sendCancelMatchmakingRequest(); }})
  {
    serverMessageHandler().setHandler<FindGameResponse>(
        [this](FindGameResponse const &response) -> bool
        {
          onFindGameResponse(response);
          return true;
        });

    serverMessageHandler().setHandler<CancelMatchmakingResponse>(
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

  void SfGameFindingScreen::goToGamePlayScreen() noexcept
  {
    // Just for test, push the SfChessScreen:
    // define the level with an array of tile indices
    constexpr unsigned const level[] = {
        0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0,
        0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0,
        0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0,
        0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0};

    SfTileMap tileMap(
        "resource/western-chess-tile-set.png",
        {200, 200},
        level,
        {8, 8});

    std::shared_ptr<GameScreen> chessScreen(new SfChessScreen(
        window(), gameDisplay_, tileMap));

    gameDisplay_->changeScreen(chessScreen);
  }

  void SfGameFindingScreen::sendCancelMatchmakingRequest() noexcept
  {
    // TODO: implement later
    // gameDisplay_->send(CancelMatchmakingRequest{});

    // std::shared_ptr<GameScreen> cancelingScreen = std::make_shared<SfMessageScreen>(
    //     window(), "Canceling...");

    // changeSubscreen(cancelingScreen);
  }

  void SfGameFindingScreen::onCancelMatchmakingResponse(CancelMatchmakingResponse const & /*response*/) noexcept
  {
    // TODO: implement later
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

      std::shared_ptr<GameScreen> messageScreen(new SfMessageScreen(
          window(), message, {"OK"}, {okButtonCallback}));

      changeSubscreen(messageScreen);
      return;
    }
  }
} // namespace bgg
