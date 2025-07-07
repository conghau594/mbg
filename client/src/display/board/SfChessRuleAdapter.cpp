// SfChessRuleAdapter.cpp

#include <boost/assert.hpp>

#include "SfChessRuleAdapter.h"
#include "model/ChessPiece.h"

namespace bgg
{
  SfChessRuleAdapter::SfChessRuleAdapter(
      std::vector<SfItemStore::Entry> itemEntries, int side) noexcept
      : itemEntries_(std::move(itemEntries)),
        rule_(side),
        squareToTileConverter_(side == ChessColor::WHITE
                                   ? squareToTileAtWhite
                                   : squareToTileAtBlack),
        tileToSquareConverter_(side == ChessColor::WHITE
                                   ? tileToSquareAtWhite
                                   : tileToSquareAtBlack)
  {
  }

  auto SfChessRuleAdapter::getItemTile(int itemIndex) const noexcept -> TileCoords
  {
    return {0, 0};
    // return itemPositions[itemIndex];
  }

  auto SfChessRuleAdapter::getItemEntry(int index) const noexcept -> SfItemStore::Entry
  {
    return itemEntries_[std::size_t(index)];
  }

  constexpr auto SfChessRuleAdapter::squareToTileAtWhite(
      ChessRule::Square const &square) noexcept -> TileCoords
  {
    BGG_VALIDATE_SQUARE(square);

    int const row = int(square[1]);
    int const col = int(square[0]);

    return TileCoords{
        col - ChessRule::Board::FIRST_COL,
        ChessRule::Board::SIDE_LENGTH - 1 + ChessRule::Board::FIRST_ROW - row};
  }

  constexpr auto SfChessRuleAdapter::squareToTileAtBlack(
      ChessRule::Square const &square) noexcept -> TileCoords
  {
    BGG_VALIDATE_SQUARE(square);

    int const row = int(square[1]);
    int const col = int(square[0]);

    return TileCoords{
        ChessRule::Board::SIDE_LENGTH - 1 + ChessRule::Board::FIRST_COL - col,
        row - ChessRule::Board::FIRST_ROW};
  }

  constexpr auto SfChessRuleAdapter::tileToSquareAtWhite(
      TileCoords const &tile) noexcept -> ChessRule::Square
  {
    BGG_VALIDATE_TILE(tile);

    return ChessRule::Square{
        char(tile.x + ChessRule::Board::FIRST_COL),
        char(ChessRule::Board::SIDE_LENGTH - 1 + ChessRule::Board::FIRST_ROW - tile.y),
        '\0'};
  }

  constexpr auto SfChessRuleAdapter::tileToSquareAtBlack(
      TileCoords const &tile) noexcept -> ChessRule::Square
  {
    BGG_VALIDATE_TILE(tile);

    return ChessRule::Square{
        char(ChessRule::Board::SIDE_LENGTH - 1 + ChessRule::Board::FIRST_COL - tile.x),
        char(tile.y + ChessRule::Board::FIRST_ROW),
        '\0'};
  }

} // namespace bgg
