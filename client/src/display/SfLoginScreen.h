// SfLoginScreen.h
#pragma once

#include "SfBaseScreen.h"

namespace iab
{
  class SfLoginScreen : public SfBaseScreen
  {
    char usernameBuffer_[128];
    char passwordBuffer_[128];
    int pressedButtonIndex_;

  public:
    SfLoginScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameService> gameService,
        std::shared_ptr<GameDisplay> gameDisplay) noexcept;

  private:
    void update(sf::Time const &elapsedTime) noexcept override;

    void onWindowEventExceptClosed(std::optional<sf::Event> const &event) noexcept override;

    void doExit() noexcept override;
    void doEnter() noexcept override;

    void layOutScreen() noexcept;
  };
} // namespace iab
