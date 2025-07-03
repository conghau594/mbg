// SfConfirmationScreen.h
#pragma once

#include <memory>
#include <functional>
#include <string>

#include "GameDisplay.h"
#include "SfMessageScreen.h"
namespace bgg
{
  class SfConfirmationScreen final : public SfMessageScreen
  {
  public:
    inline SfConfirmationScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::string message,
        std::vector<std::string> buttonLabels,
        std::vector<std::function<void()>> buttonCallbacks) noexcept;

  private:
    inline void onWindowClosed() noexcept override;
  };

  SfConfirmationScreen::SfConfirmationScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::string message,
      std::vector<std::string> buttonLabels,
      std::vector<std::function<void()>> buttonCallbacks) noexcept
      : SfMessageScreen(
            std::move(window),
            std::move(message),
            std::move(buttonLabels),
            std::move(buttonCallbacks))
  {
  }

  void SfConfirmationScreen::onWindowClosed() noexcept
  {
  }
}
