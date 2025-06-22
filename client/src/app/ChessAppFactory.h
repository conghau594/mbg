// ChessAppFactory.h
#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

#include "GameAppFactory.h"
#include "GameApp.h"

#include "connect/GameServiceSimulator.h"
#include "display/SfGameDisplay.h"
#include "display/SfGameSelectionScreen.h"

#include "model/GameType.h"
namespace iab
{
  class ChessAppFactory final : public GameAppFactory
  {
  public:
    auto createGameApp() noexcept -> GameApp override
    {
      uint32_t constexpr height = 860;
      uint32_t constexpr width = 800;
      const char *title = "Chess Game"; // "Intelligent Agent Combats"
      // TODO: consider when to use updatePeriod
      // size_t constexpr updatePeriod = 15'000; //

      std::shared_ptr<sf::RenderWindow> window(new sf::RenderWindow(
          sf::VideoMode({width, height}), title, sf::Style::Titlebar | sf::Style::Close));

      window->setMinimumSize(sf::Vector2u{300, 360});

      window->setVerticalSyncEnabled(true);
      //  window->setFramerateLimit(0);

      // create GameService object
      std::shared_ptr<GameService> gameService(new GameServiceSimulator);
      // create GameDisplay object
      std::shared_ptr<GameDisplay> gameDisplay(new SfGameDisplay(window, gameService));

      std::vector<std::string> gameTypeNames(std::begin(GameType::NAMES), std::end(GameType::NAMES));
      std::shared_ptr<GameScreen> initialScreen(new SfGameSelectionScreen(
          window, gameService, gameDisplay, gameTypeNames));

      gameDisplay->pushScreen(initialScreen);

      // return GameApp object
      return GameApp(gameDisplay);
    }
  };
} // namespace iab