// SfGameSelectionScreen.h
#pragma once

#include "SfBaseScreen.h"

namespace iab
{
  class SfGameSelectionScreen final : public SfBaseScreen
  {
    sf::Time elapsedTime_;
    int pressedButtonIndex_;

  public:
    SfGameSelectionScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameDisplay> displayContext) noexcept;

    void onExit() noexcept override;
    void onEnter() noexcept override;

  private:
    void onTimeElapsed() noexcept override;
    void onEvent(std::optional<sf::Event> const &event) noexcept override;

    void drawMenu() noexcept;
    void update(sf::Time const &elapsedTime) noexcept;
  };

} // namespace iab
