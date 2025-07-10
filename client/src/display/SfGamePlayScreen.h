// SfGamePlayScreen.h
#pragma once

#include "SfBaseScreen.h"

namespace bgg
{
  class GameDisplay;
  class SfGameBoard;
  class SfGamePlayScreen final : public SfBaseScreen
  {
    std::shared_ptr<GameDisplay> gameDisplay_;
    std::shared_ptr<SfGameBoard> gameBoard_;
    int resignRegionHeight_;
    int pressedButtonIndex_;

  public:
    SfGamePlayScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameDisplay> gameDisplay,
        std::shared_ptr<SfGameBoard> gameBoard,
        int resignRegionHeight) noexcept;

  private:
    void update(sf::Time const &elapsed) noexcept override;

    void onWindowEventExceptClosed(std::optional<sf::Event> const &event) noexcept override;

    void doEnter() noexcept override;
    void doExit() noexcept override;

    void layOutScreen() noexcept;
  };
}