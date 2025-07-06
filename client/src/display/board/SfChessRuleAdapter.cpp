// SfChessRuleAdapter.cpp

#include <boost/assert.hpp>

#include "SfChessRuleAdapter.h"
#include "model/ChessPiece.h"

namespace bgg
{
  SfChessRuleAdapter::SfChessRuleAdapter(
      std::vector<SfItemStore::Entry> itemEntries, int side) noexcept
      : itemEntries_(std::move(itemEntries)),
        rule_(side)
  {
    if (side == ChessColor::WHITE)
    {
      squareToTileConverter_ = squareToTileAtWhiteSide;
    }
    else
    {
      squareToTileConverter_ = squareToTileAtBlackSide;
    }
  }

  auto SfChessRuleAdapter::getItemEntry(int index) const noexcept -> SfItemStore::Entry
  {
    return itemEntries_[std::size_t(index)];
  }

  auto SfChessRuleAdapter::squareToTileAtWhiteSide(ChessRule::Square const &square) noexcept -> TileCoords
  {
    int const row = int(square[1]);
    int const col = int(square[0]);

    return TileCoords{
        col - ChessRule::Board::FIRST_COL,
        ChessRule::Board::SIDE_LENGTH - 1 + ChessRule::Board::FIRST_ROW - row};
  }

  auto SfChessRuleAdapter::squareToTileAtBlackSide(ChessRule::Square const &square) noexcept -> TileCoords
  {
    int const row = int(square[1]);
    int const col = int(square[0]);

    return TileCoords{
        ChessRule::Board::SIDE_LENGTH - 1 + ChessRule::Board::FIRST_COL - col,
        row - ChessRule::Board::FIRST_ROW};
  }

  auto SfChessRuleAdapter::getItemTile(int itemIndex) const noexcept -> TileCoords
  {
    return {0, 0};
    // return itemPositions[itemIndex];
  }
} // namespace bgg
