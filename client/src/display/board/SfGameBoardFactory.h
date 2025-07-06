// SfGameBoardFactory.h
#pragma once

#include <filesystem>

#include "model/GameType.h"
#include "SfChessBoard.h"
#include "SfChessTextureAtlas001.h"
#include "SfBoardPieceEnabledState.h"
#include "SfBoardPieceDisabledState.h"

namespace bgg
{
  // TODO: Make this class is specific to a single game, and a single texture.

  class SfGameBoardFactory final
  {
  public:
    [[nodiscard]]
    auto create(
        int type,
        int side,
        sf::Vector2i currentWndSize,
        sf::Vector2i paddingTopLeft,
        sf::Vector2i paddingBottomRight) const
        -> std::shared_ptr<SfGameBoard>
    {
      switch (type)
      {
      case GameType::CHESS:
        return createChessBoard(
            side,
            std::move(currentWndSize),
            std::move(paddingTopLeft),
            std::move(paddingBottomRight));
      case GameType::GOMOKU:
      default:
        return nullptr;
      }
    }

  private:
    [[nodiscard]]
    auto createChessBoard(
        int const &side,
        sf::Vector2i &&currentWndSize,
        sf::Vector2i &&paddingTopLeft,
        sf::Vector2i &&paddingBottomRight) const
        -> std::shared_ptr<SfGameBoard>
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

      std::shared_ptr<SfTextureAtlas>
          chessTextureAtlas = std::make_shared<SfChessTextureAtlas001>();

      SfTileMap tileMap(chessTextureAtlas, {8, 8}, tileLayout);
      SfItemStore itemStore(chessTextureAtlas);

      // Note that these following variables are the texture indexes of the
      // pieces in the texture atlas.

      const std::string LOWER_COLOR = (side == 0) ? "white" : "black";
      int lowerKing = (side == 0) ? ChessTextureCell::WHITE_KING : ChessTextureCell::BLACK_KING;
      int lowerRook = lowerKing + 2;
      int lowerBishop = lowerKing + 3;
      int lowerKnight = lowerKing + 4;
      int lowerPawn = lowerKing + 5;

      const std::string UPPER_COLOR = (side == 0) ? "black" : "white";
      int upperKing = (side == 0) ? ChessTextureCell::BLACK_KING : ChessTextureCell::WHITE_KING;
      int upperRook = upperKing + 2;
      int upperBishop = upperKing + 3;
      int upperKnight = upperKing + 4;
      int upperPawn = upperKing + 5;

      // Add queens and kings
      if (side == 0)
      {
        itemStore.addItem(ChessTextureCell::BLACK_QUEEN, ZOrder::SECOND_LAYER, UPPER_COLOR + " queen");
        itemStore.addItem(ChessTextureCell::WHITE_QUEEN, ZOrder::SECOND_LAYER, LOWER_COLOR + " queen");
        itemStore.addItem(ChessTextureCell::BLACK_KING, ZOrder::SECOND_LAYER, UPPER_COLOR + " king");
        itemStore.addItem(ChessTextureCell::WHITE_KING, ZOrder::SECOND_LAYER, LOWER_COLOR + " king");
      }
      else
      {
        itemStore.addItem(ChessTextureCell::BLACK_QUEEN, ZOrder::SECOND_LAYER, LOWER_COLOR + " queen");
        itemStore.addItem(ChessTextureCell::WHITE_QUEEN, ZOrder::SECOND_LAYER, UPPER_COLOR + " queen");
        itemStore.addItem(ChessTextureCell::BLACK_KING, ZOrder::SECOND_LAYER, LOWER_COLOR + " king");
        itemStore.addItem(ChessTextureCell::WHITE_KING, ZOrder::SECOND_LAYER, UPPER_COLOR + " king");
      }

      // Add left and right rooks
      itemStore.addItem(upperRook, ZOrder::SECOND_LAYER, UPPER_COLOR + " left rook");
      itemStore.addItem(upperRook, ZOrder::SECOND_LAYER, UPPER_COLOR + " right rook");
      itemStore.addItem(lowerRook, ZOrder::SECOND_LAYER, LOWER_COLOR + " left rook");
      itemStore.addItem(lowerRook, ZOrder::SECOND_LAYER, LOWER_COLOR + " right rook");

      // Add left and right knights
      itemStore.addItem(upperKnight, ZOrder::SECOND_LAYER, UPPER_COLOR + " left knight");
      itemStore.addItem(upperKnight, ZOrder::SECOND_LAYER, UPPER_COLOR + " right knight");
      itemStore.addItem(lowerKnight, ZOrder::SECOND_LAYER, LOWER_COLOR + " left knight");
      itemStore.addItem(lowerKnight, ZOrder::SECOND_LAYER, LOWER_COLOR + " right knight");

      // Add left and right bishops
      itemStore.addItem(upperBishop, ZOrder::SECOND_LAYER, UPPER_COLOR + " left bishop");
      itemStore.addItem(upperBishop, ZOrder::SECOND_LAYER, UPPER_COLOR + " right bishop");
      itemStore.addItem(lowerBishop, ZOrder::SECOND_LAYER, LOWER_COLOR + " left bishop");
      itemStore.addItem(lowerBishop, ZOrder::SECOND_LAYER, LOWER_COLOR + " right bishop");

      // Add pawns
      int constexpr PAWNS_PER_SIDE = 8;
      for (int i = 0; i < PAWNS_PER_SIDE; ++i)
      {
        std::string upperPawnName = UPPER_COLOR + " pawn " + std::to_string(i);
        itemStore.addItem(upperPawn, ZOrder::SECOND_LAYER, upperPawnName);

        std::string lowerPawnName = LOWER_COLOR + " pawn " + std::to_string(i);
        itemStore.addItem(lowerPawn, ZOrder::SECOND_LAYER, lowerPawnName);
      }

      // create chessBoard with the loaded texture atlas
      std::shared_ptr<SfGameBoard>
          chessBoard = std::make_shared<SfChessBoard>(
              currentWndSize,
              paddingTopLeft,
              paddingBottomRight,
              std::move(itemStore),
              std::move(tileMap));

      // assign the initial state of the board
      std::shared_ptr<SfBoardState> initialBoardState;
      if (side == 0)
      {
        initialBoardState = std::make_shared<SfBoardPieceEnabledState>(chessBoard);
      }
      else
      {
        initialBoardState = std::make_shared<SfBoardPieceDisabledState>(chessBoard);
      }

      chessBoard->changeState(initialBoardState);

      return chessBoard;
    }
  };
} // namespace bgg