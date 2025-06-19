// SfBaseScreen.h
#pragma once

#include <memory>
#include "GameScreen.h"

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
    std::shared_ptr<GameDisplay> gameDisplay_;
    bool shouldExit_;

  public:
    SfBaseScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameDisplay> gameDisplay) noexcept;

    void update() override final;
    void onEnter() override final;
    void onExit() override final;

  protected:
    virtual void update(sf::Time const &elapsed) = 0;

    virtual void onEvent(std::optional<sf::Event> const &event) = 0;
    virtual void onWindowClosed();

    virtual void doEnter() = 0;
    virtual void doExit() = 0;

    auto window() noexcept -> std::shared_ptr<sf::RenderWindow> const & { return window_; }
    auto gameDisplay() noexcept -> std::shared_ptr<GameDisplay> const & { return gameDisplay_; }
    auto shouldExit() noexcept -> bool & { return shouldExit_; }

    static void askExitConfirmation(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameDisplay> gameDisplay) noexcept;
  };
}