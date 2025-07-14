// ChessRuleAdapter.h
#pragma once

#include <vector>
#include <functional>

#include "IChessRuleAdapter.h"
#include "display/board/ItemStore.h"
#include "model/ChessRule.h"

#define BGG_VALIDATE_TILE(tile) BOOST_ASSERT_MSG(             \
    tile.x >= 0 && tile.x <= 7 && tile.y >= 0 && tile.y <= 7, \
    "Tile coords must be from 0 to 7")

namespace bgg
{
  class BoardItem;

  class ChessRuleAdapter final : public IChessRuleAdapter
  {
    std::shared_ptr<ItemStore> itemStore_;
    ChessRule rule_;

    ItemPlacementMap itemPlacements_; ///< Usage: itemEntries_[ChessPiece::<ENUM>]
    std::function<TileCoords(Position const &)> const positionToTileConverter_;
    std::function<Position(TileCoords const &)> const tileToPositionConverter_;

  public:
    ChessRuleAdapter(
        std::shared_ptr<ItemStore> itemStore, ChessRule rule) noexcept;

    static auto getPositionToTileConverter(std::string color) noexcept
        -> std::function<TileCoords(Position const &)>;
    static auto getTileToPositionConverter(std::string color) noexcept
        -> std::function<Position(TileCoords const &)>;

  private:
    auto positionToTile(
        Position const &position) const noexcept -> TileCoords override;
    auto tileToPosition(
        TileCoords const &tile) const noexcept -> Position override;
    void commitMove(ChessPieceMove const &move) noexcept override;

    auto getYourColor() const -> std::string const & override;

    auto getColor(TileCoords const &tile) const noexcept
        -> std::optional<std::string> override;

    auto getItemPlacements() -> ItemPlacementMap const & override;
    auto getSelectableTiles() const noexcept -> ItemPlacementMap override;
    auto getReachableTiles(TileCoords const &tile) const noexcept
        -> std::optional<ReachableTileInfo> override;

    // auto getItemType(TileCoords const &tile) const noexcept
    //     -> std::optional<int> override;
    auto getItemEntry(TileCoords const &tile) const noexcept
        -> ItemStore::Entry override;

    // auto addItemEntry(TileCoords tile, ItemStore::Entry entry) noexcept -> bool override;
  };
} // namespace bgg
