// GameBoardFactory.h
#pragma once

#include <filesystem>
#include <format>
#include <boost/assert.hpp>

#include "model/GameType.h"
#include "chess/ChessBoard.h"
#include "chess/ChessTextureAtlas001.h"
#include "chess/ChessPieceSelectableState.h"
#include "chess/ChessPieceMovedState.h"
#include "chess/ChessRuleAdapter.h"

#include "model/Piece.h"

namespace bgg
{
  // TODO: Make this class is specific to a single game, and a single texture.

  class GameBoardFactory final
  {
  public:
    //   [[nodiscard]] auto create(
    //       std::shared_ptr<ChessBoardState> chessRule,
    //       sf::IntRect const &boardRect,
    //       std::function<void(ClientRequest const &)> requestSender,
    //       bool isYourTurn) const
    //       -> std::shared_ptr<IGameBoard>
    //   {

    //     return createChessBoard(
    //         chessRule, boardRect, std::move(requestSender), isYourTurn);
    //   }

    // private:
    [[nodiscard]] auto createChessBoard(
        std::shared_ptr<ChessBoardState> chessRule,
        sf::IntRect const &boardRect,
        std::function<void(ClientRequest const &)> requestSender,
        bool isYourTurn) const
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

      std::shared_ptr<IChessRuleAdapter>
          gameRuleAdapter = std::make_shared<ChessRuleAdapter>(
              itemStore, std::move(chessRule));

      // create chessBoard with the loaded texture atlas
      std::shared_ptr<IChessBoard>
          chessBoard = std::make_shared<ChessBoard>(
              boardRect, gameRuleAdapter, tileMap, itemStore, std::move(requestSender));

      // assign the initial state of the board
      std::shared_ptr<IBoardState>
          initialBoardState = std::make_shared<ChessPieceSelectableState>(
              chessBoard, gameRuleAdapter, tileMap, itemStore);
      chessBoard->pushState(std::move(initialBoardState));

      if (!isYourTurn)
      {
        std::shared_ptr<IBoardState>
            waitingState = std::make_shared<ChessPieceMovedState>(
                chessBoard,
                std::move(gameRuleAdapter),
                std::move(tileMap),
                std::move(itemStore),
                std::nullopt);
        chessBoard->pushState(std::move(waitingState));
      }

      return chessBoard;
    }
  };
} // namespace bgg