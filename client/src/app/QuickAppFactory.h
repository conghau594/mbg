// QuickAppFactory.h
#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include "GameAppFactory.h"
#include "GameApp.h"

#include "service/MockGameService.h"
#include "display/SfGameDisplay.h"
#include "display/SfGamePlayScreen.h"
#include "display/board/SfGameBoardFactory.h"

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
            int constexpr RESIGN_REGION_HEIGHT = 40;
            int constexpr BOARD_SIDE_LENGTH = 200;
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

            std::shared_ptr<ClientEventBus>
                eventBus = std::make_shared<ClientEventBus>();
            std::shared_ptr<GameService>
                gameService = std::make_shared<MockGameService>(eventBus);
            std::shared_ptr<GameDisplay>
                gameDisplay = std::make_shared<SfGameDisplay>(window, eventBus);

            //==============
            int side = ChessColor::WHITE; // test value
            //==============
            sf::IntRect boardRect({0, RESIGN_REGION_HEIGHT}, {BOARD_SIDE_LENGTH, BOARD_SIDE_LENGTH});
            std::shared_ptr<SfGameBoard>
                chessBoard = SfGameBoardFactory().create(gameType_, side, boardRect);

            std::shared_ptr<GameScreen>
                chessScreen = std::make_shared<SfGamePlayScreen>(
                    window, gameDisplay, chessBoard, RESIGN_REGION_HEIGHT);

            gameDisplay->pushScreen(chessScreen);

            // return GameApp object
            return GameApp(gameDisplay, gameService);
        }
    };
} // namespace bgg