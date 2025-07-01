// QuickAppFactory.h
#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include "GameAppFactory.h"
#include "GameApp.h"

#include "service/MockGameService.h"
#include "display/SfGameDisplay.h"
#include "display/GameBoardScreenFactory.h"

#include "peeb/EventBus.hpp"
namespace bgg
{
  class QuickAppFactory final : public GameAppFactory
  {
    int gameType_;

  public:
    QuickAppFactory(int gameType) : gameType_(gameType) {}

    auto createGameApp() noexcept -> GameApp override
    {
      sf::Vector2u constexpr WINDOW_SIZE(800, 860);
      sf::Vector2u constexpr WINDOW_MIN_SIZE(300, 360);
      const char *WINDOW_TITLE = "Chess Game"; // "Intelligent Agent Combats"

      std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>(
          sf::VideoMode(WINDOW_SIZE),
          WINDOW_TITLE,
          sf::Style::Titlebar | sf::Style::Close);

      window->setMinimumSize(WINDOW_MIN_SIZE);

      window->setVerticalSyncEnabled(true);

      std::shared_ptr<ClientEventBus> eventBus = std::make_shared<ClientEventBus>();
      // create GameService object
      std::shared_ptr<GameService> gameService = std::make_shared<MockGameService>(eventBus);

      // create GameDisplay object
      std::shared_ptr<GameDisplay> gameDisplay = std::make_shared<SfGameDisplay>(
          window, eventBus);

      std::shared_ptr<GameScreen> initialScreen = GameBoardScreenFactory().create(
          gameType_, window, gameDisplay);

      gameDisplay->pushScreen(initialScreen);

      // return GameApp object
      return GameApp(gameDisplay, gameService);
    }
  };
} // namespace bgg