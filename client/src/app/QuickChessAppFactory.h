// QuickChessAppFactory.h
#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <fstream>

#include "IAppFactory.h"
#include "GameApp.h"

#include "service/ChessGameService.h"
#include "service/ClientRequest.h"

#include "display/GameDisplay.h"
#include "display/screen/GamePlayScreen.h"
#include "display/board/GameBoardFactory.h"

#include "peeb/EventBus.hpp"
#include "model/chess/ChessRule.h"
#include "model/PlayerType.h"
#include "base/EnvUtils.h"
namespace bgg
{
	class QuickChessAppFactory final : public IAppFactory
	{
		int gameType_;

	public:
		QuickChessAppFactory(int gameType) : gameType_(gameType) {}

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

			// parse API key from an .env file
#ifdef _DEBUG
			std::map<std::string, std::string> envMap = utils::parseEnvFile("D:/src/.env");
#else
			std::map<std::string, std::string> envMap = utils::parseEnvFile("./.env");
#endif
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
			std::shared_ptr<IDisplay>
					gameDisplay = std::make_shared<GameDisplay>(window, eventBus);

			//==============
			const std::initializer_list<std::tuple<EntityType, Side, Position>>
					TESTING_PIECE_PLACEMENTS{
							std::make_tuple(chess::KING, chess::WHITE, Position{"e1"}),
							std::make_tuple(chess::QUEEN, chess::WHITE, Position{"d4"}),
							std::make_tuple(chess::ROOK, chess::WHITE, Position{"a1"}),
							std::make_tuple(chess::ROOK, chess::WHITE, Position{"h1"}),
							std::make_tuple(chess::KNIGHT, chess::WHITE, Position{"b2"}),
							std::make_tuple(chess::KNIGHT, chess::WHITE, Position{"g2"}),
							std::make_tuple(chess::BISHOP, chess::WHITE, Position{"c2"}),
							std::make_tuple(chess::BISHOP, chess::WHITE, Position{"f2"}),
							std::make_tuple(chess::PAWN, chess::WHITE, Position{"c6"}),
							std::make_tuple(chess::PAWN, chess::WHITE, Position{"a5"}),
							std::make_tuple(chess::PAWN, chess::WHITE, Position{"a4"}),
							std::make_tuple(chess::PAWN, chess::WHITE, Position{"d2"}),
							std::make_tuple(chess::PAWN, chess::WHITE, Position{"d7"}),
							std::make_tuple(chess::PAWN, chess::WHITE, Position{"c5"}),
							std::make_tuple(chess::PAWN, chess::WHITE, Position{"b4"}),
							std::make_tuple(chess::PAWN, chess::WHITE, Position{"e2"}),
							std::make_tuple(chess::KING, chess::BLACK, Position{"g5"}),
							std::make_tuple(chess::QUEEN, chess::BLACK, Position{"f8"}),
							std::make_tuple(chess::ROOK, chess::BLACK, Position{"c8"}),
							std::make_tuple(chess::ROOK, chess::BLACK, Position{"h7"}),
							std::make_tuple(chess::KNIGHT, chess::BLACK, Position{"b7"}),
							std::make_tuple(chess::KNIGHT, chess::BLACK, Position{"g8"}),
							std::make_tuple(chess::BISHOP, chess::BLACK, Position{"e8"}),
							std::make_tuple(chess::BISHOP, chess::BLACK, Position{"f7"}),
							std::make_tuple(chess::PAWN, chess::BLACK, Position{"g6"}),
							std::make_tuple(chess::PAWN, chess::BLACK, Position{"b5"}),
							std::make_tuple(chess::PAWN, chess::BLACK, Position{"g4"}),
							std::make_tuple(chess::PAWN, chess::BLACK, Position{"g3"}),
							std::make_tuple(chess::PAWN, chess::BLACK, Position{"h6"}),
							std::make_tuple(chess::PAWN, chess::BLACK, Position{"h5"}),
							std::make_tuple(chess::PAWN, chess::BLACK, Position{"h4"}),
							std::make_tuple(chess::PAWN, chess::BLACK, Position{"h3"})};

			std::list<std::tuple<EntityType, Side, Position>>
					standardPiecePlacements(chess::STANDARD_PIECE_PLACEMENTS);
			FindGameResponse findGameResponse{
					{0, "", "Mock"}, // error code
					gameType_,
					standardPiecePlacements, // initialBoard
					chess::WHITE,						 // yourColor
					chess::WHITE						 // currentTurn
			};

			//============================
			std::shared_ptr<IGameServer>
					gameServer = std::make_shared<MultiGameServer>(eventBus);

			FindGameRequest request{
					"", GameType::CHESS, PlayerType::GEMINI, 0};
			eventBus->emit(request);

			sf::IntRect boardRect(
					{0, RESIGN_REGION_HEIGHT}, {BOARD_SIDE_LENGTH, BOARD_SIDE_LENGTH});

			std::weak_ptr<IDisplay> gameDisplayWeakPtr(gameDisplay);
			auto requestSender = [gameDisplayWeakPtr](ClientRequest const &request) noexcept
			{
				if (auto gameDisplayPtr = gameDisplayWeakPtr.lock())
				{
					gameDisplayPtr->send(request);
				}
			};

			//============================
			std::shared_ptr<IChessRule>
					chessRuleAtClient = std::make_shared<ChessRule>(standardPiecePlacements);

			std::shared_ptr<IBoardView> chessBoard = GameBoardFactory().createChessBoard(
					std::move(chessRuleAtClient),
					boardRect,
					std::move(requestSender),
					findGameResponse.yourSide,
					findGameResponse.yourSide == findGameResponse.currentTurn);

			std::shared_ptr<IScreen> chessScreen = std::make_shared<GamePlayScreen>(
					window, gameDisplay, chessBoard, RESIGN_REGION_HEIGHT);

			gameDisplay->pushScreen(chessScreen);

			// return GameApp object
			return GameApp(gameDisplay, gameServer);
		}
	};
} // namespace bgg