// ChessAppFactory.h
#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

#include "GameAppFactory.h"
#include "GameApp.h"

#include "connect/SimpleServerConnector.h"
#include "display/SfGameDisplay.h"
#include "display/SfGameSelectionScreen.h"

#include "model/GameType.h"
namespace iab
{
  class ChessAppFactory final : public GameAppFactory
  {
  public:
    GameApp createGameApp() noexcept override
    {
      // create GameDisplay object
      uint32_t constexpr height = 860;
      uint32_t constexpr width = 800;
      const char *title = "Chess Game"; // "Intelligent Agent Combats"
      // TODO: consider when to use updatePeriod
      // size_t constexpr updatePeriod = 15'000; //

      std::shared_ptr<sf::RenderWindow> window(new sf::RenderWindow(
          sf::VideoMode({width, height}), title, sf::Style::Titlebar | sf::Style::Close));

      window->setMinimumSize(sf::Vector2u{300, 360});

      // window->setVerticalSyncEnabled(false);
      //  window->setFramerateLimit(0);

      std::shared_ptr<GameDisplay> gameDisplay(new SfGameDisplay(window));

      std::vector<std::string> gameTypeNames(std::begin(GameType::NAMES), std::end(GameType::NAMES));
      std::shared_ptr<GameScreen> initialScreen(new SfGameSelectionScreen(
          window, gameDisplay, gameTypeNames));

      gameDisplay->pushScreen(initialScreen);

      // create ServerConnector object
      std::shared_ptr<ServerConnector> serverConnector(new SimpleServerConnector);

      // return GameApp object
      return GameApp(gameDisplay, serverConnector);
    }
  };
} // namespace iab