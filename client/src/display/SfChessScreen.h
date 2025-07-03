// SfChessScreen.h
#pragma once

#include "SfBaseScreen.h"

namespace bgg
{
  class GameDisplay;
  class SfGameBoard;
  class SfChessScreen final : public SfBaseScreen
  {
    std::shared_ptr<GameDisplay> gameDisplay_;
    std::shared_ptr<SfGameBoard> gameBoard_;
    unsigned resignButtonRegionHeight_;
    int pressedButtonIndex_;

  public:
    SfChessScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameDisplay> gameDisplay,
        std::shared_ptr<SfGameBoard> gameBoard,
        unsigned resignButtonRegionHeight) noexcept;

  private:
    void update(sf::Time const &elapsed) override;

    void onWindowEventExceptClosed(std::optional<sf::Event> const &event) override;

    void doEnter() override;
    void doExit() override;

    void layOutScreen() noexcept;
  };
}