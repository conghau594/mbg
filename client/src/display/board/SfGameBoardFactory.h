// SfGameBoardFactory.h
#pragma once

#include <filesystem>

#include "model/GameType.h"
#include "SfChessBoard.h"
#include "SfBoardPieceDisabledState.h"

namespace bgg
{
  // TODO: Make this class is specific to a single game, and a single texture.
  // Maybe I should name it as SfTextureAtlas? (replace the old one)
  // Lop nay neu cho input la 1 quan co thi phai tra ve vi tri texture tuong ung.
  // => dong goi viec a'nh xa quan co voi texture cell.
  // Ham: getTextureRegion(int regionName) -> sf::IntRect

  class SfGameBoardFactory final
  {
  public:
    auto create(
        int type,
        int side,
        std::shared_ptr<sf::RenderWindow> window,
        sf::Vector2i boardTopLeft) const
        -> std::shared_ptr<SfGameBoard>
    {
      switch (type)
      {
      case GameType::CHESS:
        return createChessBoard(side, std::move(window), std::move(boardTopLeft));
      case GameType::GOMOKU:
      default:
        return nullptr;
      }
    }

  private:
    auto createChessBoard(
        int const &side,
        std::shared_ptr<sf::RenderWindow> &&window,
        sf::Vector2i &&boardTopLeft) const
        -> std::shared_ptr<SfGameBoard>
    {
      sf::Vector2i constexpr mapSizeInTiles{8, 8};

      //
      std::vector<sf::Vector2i> tileLayout;
      for (int y = 0; y < mapSizeInTiles.y; ++y)
      {
        for (int x = 0; x < mapSizeInTiles.x; ++x)
        {
          int color = (x + y) % 2; ///< 0 is white, 1 is black
          tileLayout.emplace_back(color, 0);
        }
      }

      SfTextureAtlas atlas;
      // load texture atlas
      std::filesystem::path const atlasPath("resource/chess-board-texture.png");
      if (!atlas.texture.loadFromFile(atlasPath))
      {
        throw std::runtime_error("Failed to load tile set texture from " + atlasPath.string());
      }
      atlas.texture.setSmooth(true);

      // create chessBoard with the loaded texture atlas
      atlas.cellsPerRow = 20;
      atlas.cellsPerCol = 0;
      atlas.cellSize = {200, 200};
      std::shared_ptr<SfGameBoard> chessBoard = std::make_shared<SfChessBoard>(
          window, boardTopLeft, std::move(atlas), std::move(mapSizeInTiles), tileLayout);

      // place initial pieces based on the value of side: 0 is white side,
      // 1 is black side.
      // Note that these following variables are the texture indexes of the
      // pieces in the texture atlas.
      int constexpr WHITE_KING = 2;
      int constexpr BLACK_KING = 8;
      int constexpr WHITE_QUEEN = WHITE_KING + 1;
      int constexpr BLACK_QUEEN = BLACK_KING + 1;

      const std::string LOWER_COLOR = (side == 0) ? "white" : "black";
      int lowerKing = (side == 0) ? WHITE_KING : BLACK_KING;
      int lowerRook = lowerKing + 2;
      int lowerBishop = lowerKing + 3;
      int lowerKnight = lowerKing + 4;
      int lowerPawn = lowerKing + 5;

      const std::string UPPER_COLOR = (side == 0) ? "black" : "white";
      int upperKing = (side == 0) ? BLACK_KING : WHITE_KING;
      int upperRook = upperKing + 2;
      int upperBishop = upperKing + 3;
      int upperKnight = upperKing + 4;
      int upperPawn = upperKing + 5;

      // Place queens and kings
      if (side == 0)
      {
        chessBoard->addItem({3, 0}, {BLACK_QUEEN, 0}, ZOrder::SECOND_LAYER, UPPER_COLOR + " queen");
        chessBoard->addItem({3, 7}, {WHITE_QUEEN, 0}, ZOrder::SECOND_LAYER, LOWER_COLOR + " queen");
        chessBoard->addItem({4, 0}, {BLACK_KING, 0}, ZOrder::SECOND_LAYER, UPPER_COLOR + " king");
        chessBoard->addItem({4, 7}, {WHITE_KING, 0}, ZOrder::SECOND_LAYER, LOWER_COLOR + " king");
      }
      else
      {
        chessBoard->addItem({4, 7}, {BLACK_QUEEN, 0}, ZOrder::SECOND_LAYER, LOWER_COLOR + " queen");
        chessBoard->addItem({4, 0}, {WHITE_QUEEN, 0}, ZOrder::SECOND_LAYER, UPPER_COLOR + " queen");
        chessBoard->addItem({3, 7}, {BLACK_KING, 0}, ZOrder::SECOND_LAYER, LOWER_COLOR + " king");
        chessBoard->addItem({3, 0}, {WHITE_KING, 0}, ZOrder::SECOND_LAYER, UPPER_COLOR + " king");
      }

      // Place left and right rooks
      chessBoard->addItem({0, 0}, {upperRook, 0}, ZOrder::SECOND_LAYER, UPPER_COLOR + " left rook");
      chessBoard->addItem({7, 0}, {upperRook, 0}, ZOrder::SECOND_LAYER, UPPER_COLOR + " right rook");
      chessBoard->addItem({0, 7}, {lowerRook, 0}, ZOrder::SECOND_LAYER, LOWER_COLOR + " left rook");
      chessBoard->addItem({7, 7}, {lowerRook, 0}, ZOrder::SECOND_LAYER, LOWER_COLOR + " right rook");

      // Place left and right knights
      chessBoard->addItem({1, 0}, {upperKnight, 0}, ZOrder::SECOND_LAYER, UPPER_COLOR + " left knight");
      chessBoard->addItem({6, 0}, {upperKnight, 0}, ZOrder::SECOND_LAYER, UPPER_COLOR + " right knight");
      chessBoard->addItem({1, 7}, {lowerKnight, 0}, ZOrder::SECOND_LAYER, LOWER_COLOR + " left knight");
      chessBoard->addItem({6, 7}, {lowerKnight, 0}, ZOrder::SECOND_LAYER, LOWER_COLOR + " right knight");

      // Place left and right bishops
      chessBoard->addItem({2, 0}, {upperBishop, 0}, ZOrder::SECOND_LAYER, UPPER_COLOR + " left bishop");
      chessBoard->addItem({5, 0}, {upperBishop, 0}, ZOrder::SECOND_LAYER, UPPER_COLOR + " right bishop");
      chessBoard->addItem({2, 7}, {lowerBishop, 0}, ZOrder::SECOND_LAYER, LOWER_COLOR + " left bishop");
      chessBoard->addItem({5, 7}, {lowerBishop, 0}, ZOrder::SECOND_LAYER, LOWER_COLOR + " right bishop");

      // Place pawns
      int constexpr PAWNS_PER_SIDE = 8;
      for (int i = 0; i < PAWNS_PER_SIDE; ++i)
      {
        std::string upperPawnName = UPPER_COLOR + " pawn " + std::to_string(i);
        chessBoard->addItem({i, 1}, {upperPawn, 0}, ZOrder::SECOND_LAYER, upperPawnName);

        std::string lowerPawnName = LOWER_COLOR + " pawn " + std::to_string(i);
        chessBoard->addItem({i, 6}, {lowerPawn, 0}, ZOrder::SECOND_LAYER, lowerPawnName);
      }

      // assign the initial state of the board
      std::shared_ptr<SfBoardState>
          initialBoardState = std::make_shared<SfBoardPieceDisabledState>(chessBoard);

      chessBoard->changeState(initialBoardState);

      return chessBoard;
    }
  };
} // namespace bgg