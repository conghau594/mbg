// SfBlockingScreen.h
#pragma once

#include <memory>
#include <functional>
#include <string>

#include <SFML/System/Time.hpp>

#include "SfBaseScreen.h"

namespace std
{
  template <typename T>
  class optional;
}

namespace sf
{
  class RenderWindow;
  class Event;
  class Clock;
}

namespace iab
{

  class SfBlockingScreen : public SfBaseScreen
  {
    std::string message_;
    std::vector<std::string> buttonLabels_;
    std::vector<std::function<void()>> buttonCallbacks_;
    int pressedButtonIndex_;

  public:
    SfBlockingScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameDisplay> gameDisplay,
        std::string message,
        std::vector<std::string> buttonLabels,
        std::vector<std::function<void()>> buttonCallbacks) noexcept;
    //~SfBlockingScreen();

    void update(sf::Time const &elapsed) noexcept override;

    void onWindowEventExceptClosed(std::optional<sf::Event> const &event) noexcept override;

    void doExit() noexcept override;
    void doEnter() noexcept override;

  private:
    void onWindowClosed() noexcept override;
    void drawDialog() noexcept;
  };

}