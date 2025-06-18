// ChessAppFactory.h
#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

#include "GameAppFactory.h"
#include "GameApp.h"

#include "connect/SimpleServerConnector.h"
#include "display/SfGameDisplay.h"
#include "display/SfGameSelectionScreen.h"

namespace iac
{
  class ChessAppFactory final : public GameAppFactory
  {
  public:
    GameApp createGameApp() noexcept override
    {
      // create GameDisplay object
      uint32_t constexpr height = 640;
      uint32_t constexpr width = 720;
      const char *title = "Chess Game";       // "Intelligent Agent Combats"
      size_t constexpr updatePeriod = 15'000; //

      std::shared_ptr<sf::RenderWindow> window(new sf::RenderWindow(
          sf::VideoMode({width, height}), title));
      // window->setVerticalSyncEnabled(false);
      //  window->setFramerateLimit(0);

      std::shared_ptr<GameDisplay> gameDisplay(new SfGameDisplay(window));

      std::shared_ptr<GameScreen> initialScreen(new SfGameSelectionScreen(
          window, gameDisplay, updatePeriod));

      gameDisplay->pushScreen(initialScreen);

      // create ServerConnector object
      std::shared_ptr<ServerConnector> serverConnector(new SimpleServerConnector);

      // return GameApp object
      return GameApp(gameDisplay, serverConnector);
    }
  };
} // namespace iac