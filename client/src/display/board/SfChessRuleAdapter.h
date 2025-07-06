// SfChessRuleAdapter.h
#pragma once

#include <vector>

#include "SfGameRuleAdapter.h"
#include "SfItemStore.h"
#include "model/ChessRule.h"

namespace bgg
{
  class SfBoardItem;

  class SfChessRuleAdapter final : public SfGameRuleAdapter
  {
    std::vector<SfItemStore::Entry> itemEntries_;
    ChessRule rule_;
    TileCoords (*squareToTileConverter_)(ChessRule::Square const &square) noexcept;

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

    static auto squareToTileAtWhiteSide(ChessRule::Square const &square) noexcept -> TileCoords;
    static auto squareToTileAtBlackSide(ChessRule::Square const &square) noexcept -> TileCoords;
  };
} // namespace bgg
