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
}

namespace iac
{
  class GameDisplay;

  class SfBaseScreen : public GameScreen
  {
    std::shared_ptr<sf::RenderWindow> window_;
    std::shared_ptr<sf::Clock> clock_;
    std::shared_ptr<GameDisplay> displayContext_;
    size_t lag_;
    size_t const UPDATE_PERIOD_; //

  public:
    void update() override final;

  protected:
    SfBaseScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameDisplay> displayContext,
        size_t updatePeriod) noexcept;
    virtual void onPeriodicUpdate() = 0;
    virtual void onEvent(std::optional<sf::Event> event) = 0;
    virtual void render() = 0;

    auto window() noexcept -> std::shared_ptr<sf::RenderWindow> const & { return window_; }
    auto clock() noexcept -> std::shared_ptr<sf::Clock> const & { return clock_; }
    auto gameDisplay() noexcept -> std::shared_ptr<GameDisplay> const & { return displayContext_; }
    auto lag() noexcept -> size_t & { return lag_; }
    auto UPDATE_PERIOD() noexcept -> size_t const & { return UPDATE_PERIOD_; }
  };
}