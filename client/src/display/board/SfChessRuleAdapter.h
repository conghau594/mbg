// SfChessRuleAdapter.h
#pragma once

#include <vector>
#include <functional>

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
    SfItemPlacementMap itemPlacements_; ///< Usage: itemEntries_[ChessPiece::<ENUM>]
    ChessRule rule_;
    std::function<TileCoords(ChessRule::Square const &)> const squareToTileConverter_;
    std::function<ChessRule::Square(TileCoords const &)> const tileToSquareConverter_;

  public:
    SfChessRuleAdapter(ChessRule rule) noexcept;

    static auto getSquareToTileConverter(std::string color) noexcept
        -> std::function<TileCoords(ChessRule::Square const &)>;
    static auto getTileToSquareConverter(std::string color) noexcept
        -> std::function<ChessRule::Square(TileCoords const &)>;

  private:
    auto positionToTile(
        Position const &position) const noexcept -> TileCoords override;
    auto tileToPosition(
        TileCoords const &tile) const noexcept -> Position override;

    auto getSide() const -> std::string const & override;

    auto getItemPlacements() -> SfItemPlacementMap & override;
    auto getSelectableTiles() const noexcept -> SfItemPlacementMap override;
    auto getReachableTiles(TileCoords const &tile) const noexcept
        -> std::optional<SfReachableTileInfo> override;

    // auto getItemType(TileCoords const &tile) const noexcept
    //     -> std::optional<int> override;
    auto getItemEntry(TileCoords const &tile) const noexcept
        -> std::optional<SfItemStore::Entry> override;
  };
} // namespace bgg
