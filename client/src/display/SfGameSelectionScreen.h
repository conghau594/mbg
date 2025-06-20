// SfGameSelectionScreen.h
#pragma once

#include "SfBaseScreen.h"

namespace iab
{
  class SfGameSelectionScreen final : public SfBaseScreen
  {
    int pressedButtonIndex_;

  public:
    SfGameSelectionScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameDisplay> gameDisplay) noexcept;

  private:
    void update(sf::Time const &elapsedTime) noexcept override;

    void onWindowEventExceptClosed(std::optional<sf::Event> const &event) noexcept override;

    void doExit() noexcept override;
    void doEnter() noexcept override;

    void drawMenu() noexcept;
  };

} // namespace iab
