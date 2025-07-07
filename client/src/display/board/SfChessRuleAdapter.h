// SfChessRuleAdapter.h
#pragma once

#include <vector>

#include "SfGameRuleAdapter.h"
#include "SfItemStore.h"
#include "model/ChessRule.h"

#define BGG_VALIDATE_TILE(tile) BOOST_ASSERT_MSG(             \
    tile.x >= 0 && tile.x <= 7 && tile.y >= 0 && tile.y <= 7, \
    "Tile coords must be from 0 to 7")

namespace bgg
{
  class SfBoardItem;

  class SfChessRuleAdapter final : public SfGameRuleAdapter
  {
    std::vector<SfItemStore::Entry> itemEntries_;
    ChessRule rule_;
    TileCoords (*const squareToTileConverter_)(ChessRule::Square const &square);
    ChessRule::Square (*const tileToSquareConverter_)(TileCoords const &square);

  public:
    SfChessRuleAdapter(
        std::vector<SfItemStore::Entry> itemEntries, int side) noexcept;

  private:
    auto getItemPlacements() const -> std::list<SfItemPlacement> override
    {
      return std::list<SfItemPlacement>();
    }

    auto getSelectableTiles() const noexcept -> std::list<TileCoords> override
    {
      return std::list<TileCoords>();
    }

    auto getReachableTiles(TileCoords const &tile) const noexcept
        -> std::optional<SfReachableTileInfo> override
    {
      return std::optional<SfReachableTileInfo>(std::nullopt);
    }

    auto getItemIndex(TileCoords const &tile) const noexcept -> int override
    {
      // TODO:
      return 0;
    }
    auto getItemTile(int itemIndex) const noexcept -> TileCoords override;

    auto getItemEntry(int itemIndex) const noexcept -> SfItemStore::Entry override;

    static constexpr auto squareToTileAtWhite(
        ChessRule::Square const &square) noexcept -> TileCoords;
    static constexpr auto squareToTileAtBlack(
        ChessRule::Square const &square) noexcept -> TileCoords;

    static constexpr auto tileToSquareAtWhite(
        TileCoords const &tile) noexcept -> ChessRule::Square;
    static constexpr auto tileToSquareAtBlack(
        TileCoords const &tile) noexcept -> ChessRule::Square;
  };
} // namespace bgg
