// MockAppFactory.h
#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

#include "IAppFactory.h"
#include "GameApp.h"

#include "service/MockGameServer.h"
#include "display/GameDisplay.h"
#include "display/screen/LoginScreen.h"

#include "peeb/EventBus.hpp"
namespace bgg
{
  class MockAppFactory final : public IAppFactory
  {
  public:
    auto createGameApp() noexcept -> GameApp override
    {
      sf::Vector2u constexpr WINDOW_SIZE(800, 860);
      sf::Vector2u constexpr WINDOW_MIN_SIZE(300, 360);
      constexpr char WINDOW_TITLE[] = "BGG - Board Games Galore";

      // TODO: consider when to use updatePeriod
      // size_t constexpr updatePeriod = 15'000; //

      std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>(
          sf::VideoMode(WINDOW_SIZE),
          WINDOW_TITLE,
          sf::Style::Titlebar | sf::Style::Close);

      window->setMinimumSize(WINDOW_MIN_SIZE);

      window->setVerticalSyncEnabled(true);
      //  window->setFramerateLimit(0);

      std::shared_ptr<ClientEventBus>
          eventBus = std::make_shared<ClientEventBus>();
      // create IGameServer object
      std::shared_ptr<IGameServer>
          gameServer = std::make_shared<MockGameServer>(eventBus);

      // create IDisplay object
      std::shared_ptr<IDisplay> gameDisplay = std::make_shared<GameDisplay>(
          window, eventBus);

      std::shared_ptr<IScreen> initialScreen = std::make_shared<LoginScreen>(
          window, gameDisplay);

      gameDisplay->pushScreen(initialScreen);

      // return GameApp object
      return GameApp(gameDisplay, gameServer);
    }
  };
} // namespace bgg