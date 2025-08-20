// GameAppFactory.h
#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

#include "IAppFactory.h"
#include "GameApp.h"

#include "service/MultiGameServer.h"
#include "display/GameDisplay.h"
#include "display/screen/LoginScreen.h"

#include "peeb/EventBus.hpp"
namespace bgg
{
  class GameAppFactory final : public IAppFactory
  {
  public:
    auto createGameApp() noexcept -> GameApp override
    {
      int constexpr RESIGN_REGION_HEIGHT = 80;
      int constexpr BOARD_SIDE_LENGTH = 800;
      int constexpr BOARD_MIN_SIDE_LENGTH = 80;

      sf::Vector2u constexpr WINDOW_SIZE(
          BOARD_SIDE_LENGTH, BOARD_SIDE_LENGTH + RESIGN_REGION_HEIGHT);
      sf::Vector2u constexpr WINDOW_MIN_SIZE(
          BOARD_MIN_SIDE_LENGTH, BOARD_MIN_SIDE_LENGTH + RESIGN_REGION_HEIGHT);
      constexpr char WINDOW_TITLE[] = "BGG - Board Games Galore";

      std::shared_ptr<sf::RenderWindow>
          window = std::make_shared<sf::RenderWindow>(
              sf::VideoMode(WINDOW_SIZE),
              WINDOW_TITLE,
              sf::Style::Titlebar | sf::Style::Close);

      sf::View view(sf::FloatRect(
          {0.0f, 0.0f},
          {float(BOARD_SIDE_LENGTH), float(BOARD_SIDE_LENGTH + RESIGN_REGION_HEIGHT)}));

      // activate it
      window->setView(view);

      window->setMinimumSize(WINDOW_MIN_SIZE);

      window->setVerticalSyncEnabled(true);
      //  window->setFramerateLimit(0);

      std::shared_ptr<ClientEventBus>
          eventBus = std::make_shared<ClientEventBus>();

      // create IGameServer object
      std::shared_ptr<IGameServer>
          gameServer = std::make_shared<MultiGameServer>(eventBus);

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