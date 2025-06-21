// SfChessScreen.cpp

#include <SFML/Graphics/RenderWindow.hpp>

#include "SfChessScreen.h"

namespace iab
{
  SfChessScreen::SfChessScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameDisplay> gameDisplay,
      SfTileMap tileMap) noexcept
      : SfBaseScreen(window, gameDisplay),
        tileMap_(std::move(tileMap))
  {
  }

  void SfChessScreen::update(sf::Time const &elapsed)
  {
    if (elapsed == sf::Time::Zero || !isActive())
    {
      // If no time has passed or the screen should deactivate, do nothing
      return;
    }

    window()->clear();
    window()->draw(tileMap_);
    window()->display();

    // handle button presses
  }

  void SfChessScreen::onWindowEventExceptClosed(std::optional<sf::Event> const &event)
  {

    // Handle window events for the tile map
    // tileMap_.onWindowEvent(event);
  }

  void SfChessScreen::doEnter()
  {
    // tileMap_.doEnter();
  }

  void SfChessScreen::doExit()
  {
    // tileMap_.doExit();
  }
}