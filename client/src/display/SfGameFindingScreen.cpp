// SfGameFindingScreen.cpp

#include "SfGameFindingScreen.h"

#include "SfMessageScreen.h"
#include "GameDisplay.h"

#include "service/ClientEvent.h"
#include "service/ServerMessage.h"

#include "SfChessScreen.h"

namespace bgg
{
  SfMatchmakingCancelScreen::SfMatchmakingCancelScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameScreen> parentScreen,
      std::future<ServerMessage> const &futureCancelResponse)
      : SfMessageScreen(std::move(window), "Canceling...", {}, {}),
        futureCancelResponse_(futureCancelResponse),
        parentScreen_(std::move(parentScreen))
  {
  }

  void SfMatchmakingCancelScreen::update(sf::Time const &elapsed) noexcept
  {
    SfMessageScreen::update(elapsed);

    if (futureCancelResponse_.valid() &&
        futureCancelResponse_.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
      parentScreen_->changeSubscreen(nullptr);
      return;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
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
             {
               /*futureCancelResponse_ =*/gameDisplay_->send(CancelMatchmakingRequest{});
               std::shared_ptr<GameScreen> cancelingScreen(new SfMatchmakingCancelScreen(
                   this->window(), shared_from_this(), futureCancelResponse_));

               changeSubscreen(cancelingScreen);
             }})
  {
    FindGameRequest findGameRequest{"", gameType, playerType};
    /*futureFindGameResponse_ =*/gameDisplay_->send(findGameRequest);
  }

  void SfGameFindingScreen::update(sf::Time const &elapsed) noexcept
  {

    while (futureCancelResponse_.valid())
    {
      if (futureCancelResponse_.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
      {
        continue;
      }

      ServerMessage response = futureCancelResponse_.get();
      if (auto cancelResponse = response.getIf<CancelMatchmakingResponse>())
      {
        ErrorCode const &errcode = cancelResponse->errcode;
        if (errcode.value == 0) // no error -> canceled successfully
        {
          // cancel futureFindGameResponse_
          futureFindGameResponse_ = std::future<ServerMessage>();
          gameDisplay_->popScreen();
          return;
        }
        else
        {
          std::string message = "Failed to cancel matchmaking\n(" +
                                errcode.message + " (" +
                                std::to_string(errcode.value) + "))";
          break;
        }
      }
    }

    if (futureFindGameResponse_.valid() &&
        futureFindGameResponse_.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
      ServerMessage response = futureFindGameResponse_.get();
      if (auto findGameResponse = response.getIf<FindGameResponse>())
      {
        ErrorCode const &errcode = findGameResponse->errcode;
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
            gameDisplay_->popScreen();
          };

          std::shared_ptr<GameScreen> messageScreen(new SfMessageScreen(
              window(), message, {"OK"}, {okButtonCallback}));

          changeSubscreen(messageScreen);
          return;
        }
      }
    }

    if (isCancelButtonPressed_) // the Cancel button is pressed only once
    {
      /*futureCancelResponse_ = */ gameDisplay_->send(CancelMatchmakingRequest{});
      std::shared_ptr<GameScreen> cancelScreen(new SfMatchmakingCancelScreen(
          window(), shared_from_this(), futureCancelResponse_));

      changeSubscreen(cancelScreen);
      isCancelButtonPressed_ = false;
      return;
    }

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
    //=============================================================================
  }
} // namespace bgg
