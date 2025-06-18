// SfBaseScreen.h
#pragma once

#include "GameScreen.h"
#include <memory>

namespace std
{
  template <typename T>
  class optional;
}

namespace sf
{
  class RenderWindow;
  class Clock;
  class Event;
  class Time;
}

namespace iab
{
  class GameDisplay;

  class SfBaseScreen : public GameScreen
  {
    std::shared_ptr<sf::RenderWindow> window_;
    std::shared_ptr<sf::Clock> clock_;
    std::shared_ptr<GameDisplay> displayContext_;

  public:
    void update() override final;

  protected:
    SfBaseScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameDisplay> displayContext) noexcept;
    virtual void onTimeElapsed() = 0;
    virtual void onEvent(std::optional<sf::Event> const &event) = 0;

    auto window() noexcept -> std::shared_ptr<sf::RenderWindow> const & { return window_; }
    auto clock() noexcept -> std::shared_ptr<sf::Clock> const & { return clock_; }
    auto gameDisplay() noexcept -> std::shared_ptr<GameDisplay> const & { return displayContext_; }

    void askExitConfirmation() noexcept;
  };
}