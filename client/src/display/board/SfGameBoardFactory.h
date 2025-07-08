// SfGameBoardFactory.h
#pragma once

#include <filesystem>
#include <format>
#include <boost/assert.hpp>

#include "model/GameType.h"
#include "SfChessBoard.h"
#include "SfChessTextureAtlas001.h"
#include "SfBoardPieceEnabledState.h"
#include "SfBoardPieceDisabledState.h"
#include "SfChessRuleAdapter.h"

#include "model/ChessPiece.h"

namespace bgg
{
  // TODO: Make this class is specific to a single game, and a single texture.

  class SfGameBoardFactory final
  {
  public:
    [[nodiscard]]
    auto create(int type, int side, sf::IntRect const &boardRect) const
        -> std::shared_ptr<SfGameBoard>
    {
      switch (type)
      {
      case GameType::CHESS:
        return createChessBoard(side, boardRect);
      case GameType::GOMOKU:
      default:
        return nullptr;
      }
    }

  private:
    [[nodiscard]]
    auto createChessBoard(
        int const &side, sf::IntRect const &boardRect) const
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

      auto tileMap = std::make_shared<SfTileMap>(
          chessTextureAtlas, sf::Vector2i{8, 8}, tileLayout);

      auto itemStore = std::make_shared<SfItemStore>(chessTextureAtlas);

      // this vector obj maps each ChessPiece enum to a item entry
      std::vector<SfItemStore::Entry> itemEntries(
          std::size_t(ChessPiece::COUNT), *itemStore);

      // this vector obj maps each ChessPiece enum to a coresponding texture index
      std::vector<int> textureIndexes{
          ChessTextureCell::WHITE_KING,
          ChessTextureCell::WHITE_QUEEN,
          ChessTextureCell::WHITE_ROOK,
          ChessTextureCell::WHITE_ROOK,
          ChessTextureCell::WHITE_BISHOP,
          ChessTextureCell::WHITE_BISHOP,
          ChessTextureCell::WHITE_KNIGHT,
          ChessTextureCell::WHITE_KNIGHT,
          ChessTextureCell::WHITE_PAWN,
          ChessTextureCell::WHITE_PAWN,
          ChessTextureCell::WHITE_PAWN,
          ChessTextureCell::WHITE_PAWN,
          ChessTextureCell::WHITE_PAWN,
          ChessTextureCell::WHITE_PAWN,
          ChessTextureCell::WHITE_PAWN,
          ChessTextureCell::WHITE_PAWN,

          ChessTextureCell::BLACK_KING,
          ChessTextureCell::BLACK_QUEEN,
          ChessTextureCell::BLACK_ROOK,
          ChessTextureCell::BLACK_ROOK,
          ChessTextureCell::BLACK_BISHOP,
          ChessTextureCell::BLACK_BISHOP,
          ChessTextureCell::BLACK_KNIGHT,
          ChessTextureCell::BLACK_KNIGHT,
          ChessTextureCell::BLACK_PAWN,
          ChessTextureCell::BLACK_PAWN,
          ChessTextureCell::BLACK_PAWN,
          ChessTextureCell::BLACK_PAWN,
          ChessTextureCell::BLACK_PAWN,
          ChessTextureCell::BLACK_PAWN,
          ChessTextureCell::BLACK_PAWN,
          ChessTextureCell::BLACK_PAWN};

      // add piece items to itemStore, also assign the returned entry
      for (int i = 0; i < ChessPiece::COUNT; ++i)
      {
        std::optional<std::string> itemName = ChessPiece::toString(i);
        BOOST_ASSERT_MSG(
            itemName,
            std::format("Enum ChessPiece with value {} should have a associated string", i)
                .c_str());

        itemEntries[std::size_t(i)] = itemStore->addItem(
            textureIndexes[std::size_t(i)], ZOrder::SECOND_LAYER, itemName.value());
      }

      // assert valid item entries before creating chess board
      BOOST_ASSERT_MSG(int(itemEntries.size()) == ChessPiece::COUNT,
                       "itemEntries_ size must be equal to"
                       " the number of chess pieces (32)");
      for (int i = 0; i < ChessPiece::COUNT; ++i)
      {
        BOOST_ASSERT_MSG(!itemEntries[std::size_t(i)].isNull(),
                         "itemEntries_ should not contain null item entry");
      }

      std::shared_ptr<SfGameRuleAdapter> gameRule = std::make_shared<SfChessRuleAdapter>(
          std::move(itemEntries), side);

      // create chessBoard with the loaded texture atlas
      std::shared_ptr<SfGameBoard>
          chessBoard = std::make_shared<SfChessBoard>(
              boardRect, gameRule, tileMap, itemStore);

      // assign the initial state of the board
      std::shared_ptr<SfBoardState> initialBoardState;

      if (side == ChessColor::WHITE)
      {
        initialBoardState = std::make_shared<SfBoardPieceEnabledState>(
            chessBoard, std::move(gameRule), std::move(tileMap), std::move(itemStore));
      }
      else
      {
        initialBoardState = std::make_shared<SfBoardPieceDisabledState>(
            chessBoard, std::move(gameRule), std::move(tileMap), std::move(itemStore));
      }
      chessBoard->changeState(initialBoardState);

      return chessBoard;
    }
  };
} // namespace bgg