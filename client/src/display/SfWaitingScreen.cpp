// SfWaitingScreen.cpp

#include "SfWaitingScreen.h"
#include "GameDisplay.h"
#include "SfBlockingScreen.h"

namespace iab
{
  SfWaitingScreen::SfWaitingScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameService> gameService,
      std::shared_ptr<ScreenManager> screenMgr,
      std::string const &message,
      std::vector<std::string> buttonLabels,
      std::vector<std::function<void()>> buttonCallbacks) noexcept
      : SfBaseScreen(window, gameService, screenMgr),
        blockingScreen_(new SfBlockingScreen(
            window, gameService, screenMgr, message, buttonLabels, buttonCallbacks))
  {
  }

  void SfWaitingScreen::doExit() noexcept
  {
    blockingScreen_->doExit();
  }

  void SfWaitingScreen::doEnter() noexcept
  {
    blockingScreen_->doEnter();
  }

  void SfWaitingScreen::update(sf::Time const &elapsed) noexcept
  {
    blockingScreen_->update(elapsed);
  }

  void SfWaitingScreen::onWindowEventExceptClosed(std::optional<sf::Event> const &event) noexcept
  {
    blockingScreen_->onWindowEventExceptClosed(event);
  }
};
