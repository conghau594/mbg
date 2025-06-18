// SfPauseScreen.h
#pragma once

#include <memory>
#include <functional>

#include "GameScreen.h"

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
  class SfPauseScreen final : public GameScreen
  {
    std::string message_;
    std::vector<std::string> buttonLabels_;
    std::vector<std::function<void()>> buttonCallbacks_;
    std::shared_ptr<sf::RenderWindow> window_;
    std::shared_ptr<sf::Clock> clock_;
    sf::Time elapsedTime_;
    int pressedButtonIndex_;

  public:
    SfPauseScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::string message,
        std::vector<std::string> buttonLabels,
        std::vector<std::function<void()>> buttonCallbacks) noexcept;
    //~SfPauseScreen();

    void update() noexcept override;
    void onEnter() noexcept override;
    void onExit() noexcept override;

  private:
    void drawDialog() noexcept;
  };

}