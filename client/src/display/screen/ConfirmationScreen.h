// ConfirmationScreen.h
#pragma once

#include <memory>
#include <functional>
#include <string>

#include "MessageScreen.h"
#include "base/Logger.h"

namespace bgg
{
  /**
   *
   * This screen differs from `MessageScreen` in that it does not allow
   * closing window.
   */
  class ConfirmationScreen final : public MessageScreen
  {
  public:
    inline ConfirmationScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::string message,
        std::vector<std::string> buttonLabels,
        std::vector<std::function<void()>> buttonCallbacks) noexcept;

    ~ConfirmationScreen() noexcept
    {
      SPDLOG_DEBUG("ConfirmationScreen destructor called");
    }

  private:
    inline void onWindowClosed() noexcept override;
  };

  ConfirmationScreen::ConfirmationScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::string message,
      std::vector<std::string> buttonLabels,
      std::vector<std::function<void()>> buttonCallbacks) noexcept
      : MessageScreen(
            std::move(window),
            std::move(message),
            std::move(buttonLabels),
            std::move(buttonCallbacks))
  {
  }

  void ConfirmationScreen::onWindowClosed() noexcept
  {
  }
}
