// SfGameSelectionScreen.h
#pragma once

#include <vector>

#include "SfBaseScreen.h"

namespace iab
{
  class SfGameSelectionScreen final : public SfBaseScreen
  {
    std::shared_ptr<GameDisplay> gameDisplay_;
    std::vector<std::string> gameNames_;
    int pressedButtonIndex_;

  public:
    SfGameSelectionScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameDisplay> gameDisplay,
        std::vector<std::string> const &gameNames) noexcept;

  private:
    void update(sf::Time const &elapsedTime) noexcept override;

    void onWindowEventExceptClosed(std::optional<sf::Event> const &event) noexcept override;

    void doExit() noexcept override;
    void doEnter() noexcept override;

    void layOutScreen() noexcept;
  };

} // namespace iab
