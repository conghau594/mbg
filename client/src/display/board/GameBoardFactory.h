// GameBoardFactory.h
#pragma once

#include <filesystem>
#include <format>
#include <boost/assert.hpp>

#include "model/GameType.h"
#include "chess/ChessBoard.h"
#include "chess/ChessTextureAtlas001.h"
#include "chess/ChessPieceSelectableState.h"
#include "chess/ChessRuleAdapter.h"

#include "model/Piece.h"

namespace bgg
{
  // TODO: Make this class is specific to a single game, and a single texture.

  class GameBoardFactory final
  {
  public:
    [[nodiscard]]
    auto create(
        FindGameResponse const &findGameResponse,
        sf::IntRect const &boardRect,
        std::function<void(ClientRequest const &)> requestSender) const
        -> std::shared_ptr<IGameBoard>
    {
      switch (findGameResponse.gameType)
      {
      case GameType::CHESS:
        return createChessBoard(findGameResponse, boardRect, std::move(requestSender));
      case GameType::GOMOKU:
      default:
        return nullptr;
      }
    }

  private:
    [[nodiscard]]
    auto createChessBoard(
        FindGameResponse const &findGameResponse,
        sf::IntRect const &boardRect,
        std::function<void(ClientRequest const &)> &&requestSender) const
        -> std::shared_ptr<IGameBoard>
    {
      sf::Vector2i constexpr mapSizeInTiles{8, 8};

      // 0 is WHITE_SQUARE, 1 is BLACK_SQUARE
      std::vector<int> tileLayout{
          0, 1, 0, 1, 0, 1, 0, 1,
          1, 0, 1, 0, 1, 0, 1, 0,
          0, 1, 0, 1, 0, 1, 0, 1,
          1, 0, 1, 0, 1, 0, 1, 0,
          0, 1, 0, 1, 0, 1, 0, 1,
          1, 0, 1, 0, 1, 0, 1, 0,
          0, 1, 0, 1, 0, 1, 0, 1,
          1, 0, 1, 0, 1, 0, 1, 0};

      std::shared_ptr<BaseTextureAtlas>
          chessTextureAtlas = std::make_shared<ChessTextureAtlas001>();

      auto tileMap = std::make_shared<TileMap>(
          chessTextureAtlas, sf::Vector2i{8, 8}, tileLayout);

      auto itemStore = std::make_shared<ItemStore>(chessTextureAtlas);

      ChessRule chessRule(findGameResponse.initialBoard, findGameResponse.yourSide);
      // TODO: need to consider how to initiate the IChessRuleAdapter with
      //       findGameResponse.initialBoard
      std::shared_ptr<IChessRuleAdapter>
          gameRuleAdapter = std::make_shared<ChessRuleAdapter>(itemStore, std::move(chessRule));

      // create chessBoard with the loaded texture atlas
      std::shared_ptr<IChessBoard>
          chessBoard = std::make_shared<ChessBoard>(
              boardRect, gameRuleAdapter, tileMap, itemStore, requestSender);

      // assign the initial state of the board
      std::shared_ptr<IBoardState> initialBoardState;

      if (findGameResponse.yourTurn == findGameResponse.currentTurn)
      {
        initialBoardState = std::make_shared<ChessPieceSelectableState>(
            chessBoard,
            std::move(gameRuleAdapter),
            std::move(tileMap),
            std::move(itemStore));

        chessBoard->pushState(std::move(initialBoardState));
      }

      return chessBoard;
    }
  };
} // namespace bgg