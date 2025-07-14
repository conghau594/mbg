// ChessAppFactory.h
#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

#include "GameAppFactory.h"
#include "GameApp.h"

#include "service/MockGameService.h"
#include "display/GameDisplay.h"
#include "display/screen/LoginScreen.h"

#include "peeb/EventBus.hpp"
namespace bgg
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

      std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>(
          sf::VideoMode(WINDOW_SIZE),
          WINDOW_TITLE,
          sf::Style::Titlebar | sf::Style::Close);

      window->setMinimumSize(WINDOW_MIN_SIZE);

      window->setVerticalSyncEnabled(true);
      //  window->setFramerateLimit(0);

      std::shared_ptr<ClientEventBus> eventBus = std::make_shared<ClientEventBus>();
      // create GameService object
      std::shared_ptr<GameService> gameService = std::make_shared<MockGameService>(eventBus);

      // create IDisplay object
      std::shared_ptr<IDisplay> gameDisplay = std::make_shared<GameDisplay>(
          window, eventBus);

      std::shared_ptr<IScreen> initialScreen = std::make_shared<LoginScreen>(
          window, gameDisplay);

      gameDisplay->pushScreen(initialScreen);

      // return GameApp object
      return GameApp(gameDisplay, gameService);
    }
  };
} // namespace bgg