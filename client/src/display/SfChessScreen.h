// SfChessScreen.h
#pragma once

#include "SfBaseScreen.h"
#include "SfTileMap.h"

namespace iab
{
  class GameDisplay;
  class SfChessScreen : public SfBaseScreen
  {
    std::shared_ptr<GameDisplay> gameDisplay_;
    SfTileMap tileMap_;
    sf::Vector2u mapRegionTopLeft_;
    sf::Vector2u mapRegionBotRight_;
    int pressedButtonIndex_;

  public:
    SfChessScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameService> gameService,
        std::shared_ptr<GameDisplay> gameDisplay,
        SfTileMap tileMap) noexcept;

  private:
    void update(sf::Time const &elapsed) override;

    void onWindowEventExceptClosed(std::optional<sf::Event> const &event) override;

    void doEnter() override;
    void doExit() override;

    void layOutScreen() noexcept;
  };
}