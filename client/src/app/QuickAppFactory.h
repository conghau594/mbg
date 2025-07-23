// QuickAppFactory.h
#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <fstream>

#include "GameAppFactory.h"
#include "GameApp.h"

#include "service/ChessGameService.h"
#include "display/GameDisplay.h"
#include "display/screen/GamePlayScreen.h"
#include "display/board/GameBoardFactory.h"

#include "peeb/EventBus.hpp"
#include "model/chess/ChessBoardState.h"
#include "base/EnvUtils.h"
namespace bgg
{
	class QuickAppFactory final : public GameAppFactory
	{
		int gameType_;

	public:
		QuickAppFactory(int gameType) : gameType_(gameType) {}

		auto createGameApp() noexcept -> GameApp override
		{
			int constexpr RESIGN_REGION_HEIGHT = 80;
			int constexpr BOARD_SIDE_LENGTH = 1000;
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

			// parse API key from an .env file
			std::map<std::string, std::string> envMap = utils::parseEnvFile("E:/src/.env");
			auto envIter = envMap.find("GEMINI_API_KEY");
			std::string geminiApiKey;
			if (envIter == envMap.end())
			{
				SPDLOG_ERROR("GEMINI_API_KEY is not found in the .env file!");
			}
			else
			{
				geminiApiKey = envIter->second;
			}
			std::shared_ptr<ClientEventBus>
					eventBus = std::make_shared<ClientEventBus>();
			std::shared_ptr<GameService>
					gameService = std::make_shared<ChessGameService>(
							std::move(geminiApiKey), eventBus);
			std::shared_ptr<IDisplay>
					gameDisplay = std::make_shared<GameDisplay>(window, eventBus);

			//==============
			std::pair<Position, Piece> TESTING_PLACEMENTS[ChessRule::PIECE_COUNT]{
					{Position{"e1"}, Piece{ChessRule::KING, Color::WHITE}},
					{Position{"d4"}, Piece{ChessRule::QUEEN, Color::WHITE}},
					{Position{"a1"}, Piece{ChessRule::ROOK, Color::WHITE}},
					{Position{"h1"}, Piece{ChessRule::ROOK, Color::WHITE}},
					{Position{"b2"}, Piece{ChessRule::KNIGHT, Color::WHITE}},
					{Position{"g2"}, Piece{ChessRule::KNIGHT, Color::WHITE}},
					{Position{"c2"}, Piece{ChessRule::BISHOP, Color::WHITE}},
					{Position{"f2"}, Piece{ChessRule::BISHOP, Color::WHITE}},
					{Position{"c6"}, Piece{ChessRule::PAWN, Color::WHITE}},
					{Position{"a5"}, Piece{ChessRule::PAWN, Color::WHITE}},
					{Position{"a4"}, Piece{ChessRule::PAWN, Color::WHITE}},
					{Position{"d2"}, Piece{ChessRule::PAWN, Color::WHITE}},
					{Position{"d7"}, Piece{ChessRule::PAWN, Color::WHITE}},
					{Position{"c5"}, Piece{ChessRule::PAWN, Color::WHITE}},
					{Position{"b4"}, Piece{ChessRule::PAWN, Color::WHITE}},
					{Position{"e2"}, Piece{ChessRule::PAWN, Color::WHITE}},

					{Position{"g5"}, Piece{ChessRule::KING, Color::BLACK}},
					{Position{"f8"}, Piece{ChessRule::QUEEN, Color::BLACK}},
					{Position{"c8"}, Piece{ChessRule::ROOK, Color::BLACK}},
					{Position{"h7"}, Piece{ChessRule::ROOK, Color::BLACK}},
					{Position{"b7"}, Piece{ChessRule::KNIGHT, Color::BLACK}},
					{Position{"g8"}, Piece{ChessRule::KNIGHT, Color::BLACK}},
					{Position{"e8"}, Piece{ChessRule::BISHOP, Color::BLACK}},
					{Position{"f7"}, Piece{ChessRule::BISHOP, Color::BLACK}},
					{Position{"g6"}, Piece{ChessRule::PAWN, Color::BLACK}},
					{Position{"b5"}, Piece{ChessRule::PAWN, Color::BLACK}},
					{Position{"g4"}, Piece{ChessRule::PAWN, Color::BLACK}},
					{Position{"g3"}, Piece{ChessRule::PAWN, Color::BLACK}},
					{Position{"h6"}, Piece{ChessRule::PAWN, Color::BLACK}},
					{Position{"h5"}, Piece{ChessRule::PAWN, Color::BLACK}},
					{Position{"h4"}, Piece{ChessRule::PAWN, Color::BLACK}},
					{Position{"h3"}, Piece{ChessRule::PAWN, Color::BLACK}},
			};

			std::map<Position, Piece> initialPlacements(
					TESTING_PLACEMENTS, TESTING_PLACEMENTS + ChessRule::PIECE_COUNT);

			std::map<Position, Piece> standardPlacements(
					ChessRule::INITIAL_PLACEMENTS);

			FindGameResponse findGameResponse{
					{0, "", "Mock"}, // error code
					gameType_,
					std::move(standardPlacements), // initialBoard -> empty
					Color::WHITE,									 // yourSide
					Color::WHITE,									 // yourTurn
					Color::WHITE									 // currentTurn
			};
			//==============

			sf::IntRect boardRect(
					{0, RESIGN_REGION_HEIGHT}, {BOARD_SIDE_LENGTH, BOARD_SIDE_LENGTH});

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