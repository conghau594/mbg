// SfMessageScreen.h
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

namespace bgg
{

  class SfMessageScreen : public SfBaseScreen
  {
    std::string message_;
    std::vector<std::string> buttonLabels_;
    std::vector<std::function<void()>> buttonCallbacks_;
    int pressedButtonIndex_;

  public:
    SfMessageScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::string message,
        std::vector<std::string> buttonLabels = {},
        std::vector<std::function<void()>> buttonCallbacks = {}) noexcept;

  protected:
    void update(sf::Time const &elapsed) noexcept override;

    void onWindowEventExceptClosed(std::optional<sf::Event> const &event) noexcept override;

    void doExit() noexcept override;
    void doEnter() noexcept override;

    void layOutScreen() noexcept;
  };

}