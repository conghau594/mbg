// SfConfirmationScreen.h
#pragma once

#include <memory>
#include <functional>
#include <string>

#include "GameDisplay.h"
#include "SfBlockingScreen.h"
namespace iab
{
  class SfConfirmationScreen final : public SfBlockingScreen
  {
  public:
    inline SfConfirmationScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameService> gameService,
        std::shared_ptr<GameDisplay> gameDisplay,
        std::string const &message,
        std::vector<std::string> buttonLabels,
        std::vector<std::function<void()>> buttonCallbacks) noexcept;

  private:
    inline void onWindowClosed() noexcept override;
  };

  SfConfirmationScreen::SfConfirmationScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameService> gameService,
      std::shared_ptr<GameDisplay> gameDisplay,
      std::string const &message,
      std::vector<std::string> buttonLabels,
      std::vector<std::function<void()>> buttonCallbacks) noexcept
      : SfBlockingScreen(
            window, gameService, gameDisplay, message, buttonLabels, buttonCallbacks)
  {
  }

  void SfConfirmationScreen::onWindowClosed() noexcept
  {
  }
}
