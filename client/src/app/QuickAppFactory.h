// QuickAppFactory.h
#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include "GameAppFactory.h"
#include "GameApp.h"

#include "service/MockGameService.h"
#include "display/GameDisplay.h"
#include "display/screen/GamePlayScreen.h"
#include "display/board/GameBoardFactory.h"

#include "peeb/EventBus.hpp"
#include "model/chess/ChessBoardState.h"
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
			int constexpr BOARD_SIDE_LENGTH = 1200;
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
			std::shared_ptr<IDisplay>
					gameDisplay = std::make_shared<GameDisplay>(window, eventBus);

			//==============
			std::pair<Position, Piece> TESTING_PLACEMENTS[ChessRule::PIECE_COUNT]{
					{Position{"e1"}, Piece{ChessRule::KING, Color::WHITE}},
					{Position{"d4"}, Piece{ChessRule::QUEEN, Color::WHITE}},
					{Position{"a2"}, Piece{ChessRule::ROOK, Color::WHITE}},
					{Position{"h2"}, Piece{ChessRule::ROOK, Color::WHITE}},
					{Position{"b2"}, Piece{ChessRule::KNIGHT, Color::WHITE}},
					{Position{"g2"}, Piece{ChessRule::KNIGHT, Color::WHITE}},
					{Position{"c2"}, Piece{ChessRule::BISHOP, Color::WHITE}},
					{Position{"f2"}, Piece{ChessRule::BISHOP, Color::WHITE}},
					{Position{"a6"}, Piece{ChessRule::PAWN, Color::WHITE}},
					{Position{"a5"}, Piece{ChessRule::PAWN, Color::WHITE}},
					{Position{"a4"}, Piece{ChessRule::PAWN, Color::WHITE}},
					{Position{"a3"}, Piece{ChessRule::PAWN, Color::WHITE}},
					{Position{"b6"}, Piece{ChessRule::PAWN, Color::WHITE}},
					{Position{"b5"}, Piece{ChessRule::PAWN, Color::WHITE}},
					{Position{"b4"}, Piece{ChessRule::PAWN, Color::WHITE}},
					{Position{"b3"}, Piece{ChessRule::PAWN, Color::WHITE}},

					{Position{"e7"}, Piece{ChessRule::KING, Color::BLACK}},
					{Position{"d7"}, Piece{ChessRule::QUEEN, Color::BLACK}},
					{Position{"a7"}, Piece{ChessRule::ROOK, Color::BLACK}},
					{Position{"h7"}, Piece{ChessRule::ROOK, Color::BLACK}},
					{Position{"b7"}, Piece{ChessRule::KNIGHT, Color::BLACK}},
					{Position{"g7"}, Piece{ChessRule::KNIGHT, Color::BLACK}},
					{Position{"c7"}, Piece{ChessRule::BISHOP, Color::BLACK}},
					{Position{"f7"}, Piece{ChessRule::BISHOP, Color::BLACK}},
					{Position{"g6"}, Piece{ChessRule::PAWN, Color::BLACK}},
					{Position{"g5"}, Piece{ChessRule::PAWN, Color::BLACK}},
					{Position{"g4"}, Piece{ChessRule::PAWN, Color::BLACK}},
					{Position{"g3"}, Piece{ChessRule::PAWN, Color::BLACK}},
					{Position{"h6"}, Piece{ChessRule::PAWN, Color::BLACK}},
					{Position{"h5"}, Piece{ChessRule::PAWN, Color::BLACK}},
					{Position{"h4"}, Piece{ChessRule::PAWN, Color::BLACK}},
					{Position{"h3"}, Piece{ChessRule::PAWN, Color::BLACK}},
			};

			std::map<Position, Piece> initialPlacements(
					TESTING_PLACEMENTS, TESTING_PLACEMENTS + ChessRule::PIECE_COUNT);
			FindGameResponse findGameResponse{
					{0, "", "Mock"}, // error code
					gameType_,
					// 0, // playerType -> empty

					std::move(initialPlacements), // initialBoard -> empty. TODO: should make this work
					Color::WHITE,									// yourSide
					0,														// yourTurn
					0															// currentTurn
			};
			//==============

			sf::IntRect boardRect(
					{0, RESIGN_REGION_HEIGHT},
					{BOARD_SIDE_LENGTH, BOARD_SIDE_LENGTH});

			auto requestSender = [gameDisplay](ClientRequest const &request) noexcept
			{
				gameDisplay->send(request);
			};

			std::shared_ptr<IGameBoard> chessBoard = GameBoardFactory().create(
					findGameResponse, boardRect, std::move(requestSender));

			std::shared_ptr<IScreen> chessScreen = std::make_shared<GamePlayScreen>(
					window, gameDisplay, chessBoard, RESIGN_REGION_HEIGHT);

			gameDisplay->pushScreen(chessScreen);

			// return GameApp object
			return GameApp(gameDisplay, gameService);
		}
	};
} // namespace bgg