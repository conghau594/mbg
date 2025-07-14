// ChessRuleAdapter.h
#pragma once

#include <vector>
#include <functional>

#include "display/board/IGameRuleAdapter.h"
#include "display/board/ItemStore.h"
#include "model/ChessRule.h"

#define BGG_VALIDATE_TILE(tile) BOOST_ASSERT_MSG(             \
    tile.x >= 0 && tile.x <= 7 && tile.y >= 0 && tile.y <= 7, \
    "Tile coords must be from 0 to 7")

namespace bgg
{
  class BoardItem;

  class ChessRuleAdapter final : public IGameRuleAdapter
  {
    ItemPlacementMap itemPlacements_; ///< Usage: itemEntries_[ChessPiece::<ENUM>]
    ChessRule rule_;
    std::function<TileCoords(ChessRule::Square const &)> const squareToTileConverter_;
    std::function<ChessRule::Square(TileCoords const &)> const tileToSquareConverter_;

  public:
    ChessRuleAdapter(ChessRule rule) noexcept;

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

    auto getItemPlacements() -> ItemPlacementMap & override;
    auto getSelectableTiles() const noexcept -> ItemPlacementMap override;
    auto getReachableTiles(TileCoords const &tile) const noexcept
        -> std::optional<ReachableTileInfo> override;

    // auto getItemType(TileCoords const &tile) const noexcept
    //     -> std::optional<int> override;
    auto getItemEntry(TileCoords const &tile) const noexcept
        -> std::optional<ItemStore::Entry> override;
  };
} // namespace bgg
