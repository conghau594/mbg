// ChessAppFactory.h
#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

#include "GameAppFactory.h"
#include "GameApp.h"

#include "service/MockGameService.h"
#include "display/SfGameDisplay.h"
#include "display/SfLoginScreen.h"

namespace iab
{
  class ChessAppFactory final : public GameAppFactory
  {
  public:
    auto createGameApp() noexcept -> GameApp override
    {
      sf::Vector2u constexpr WINDOW_SIZE(800, 860);
      sf::Vector2u constexpr WINDOW_MIN_SIZE(300, 360);
      const char *WINDOW_TITLE = "Chess Game"; // "Intelligent Agent Combats"
      // TODO: consider when to use updatePeriod
      // size_t constexpr updatePeriod = 15'000; //

      std::shared_ptr<sf::RenderWindow> window(new sf::RenderWindow(
          sf::VideoMode(WINDOW_SIZE),
          WINDOW_TITLE,
          sf::Style::Titlebar | sf::Style::Close));

      window->setMinimumSize(WINDOW_MIN_SIZE);

      window->setVerticalSyncEnabled(true);
      //  window->setFramerateLimit(0);

      // create GameService object
      std::shared_ptr<GameService> gameService(new MockGameService);
      // create GameDisplay object
      std::shared_ptr<GameDisplay> gameDisplay(
          new SfGameDisplay(window, gameService));

      std::shared_ptr<GameScreen> initialScreen(new SfLoginScreen(
          window, gameDisplay));

      gameDisplay->pushScreen(initialScreen);

      // return GameApp object
      return GameApp(gameDisplay);
    }
  };
} // namespace iab