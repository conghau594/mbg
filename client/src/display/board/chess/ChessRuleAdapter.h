// ChessRuleAdapter.h
#pragma once

#include <vector>
#include <functional>

#include "IChessRuleAdapter.h"
#include "display/board/ItemStore.h"
#include "model/chess/ChessBoardState.h"

#define BGG_VALIDATE_TILE(tile) BOOST_ASSERT_MSG(             \
    tile.x >= 0 && tile.x <= 7 && tile.y >= 0 && tile.y <= 7, \
    "Tile coords must be from 0 to 7")

namespace bgg
{
    class BoardItem;

    class ChessRuleAdapter final : public IChessRuleAdapter
    {
        std::shared_ptr<ChessBoardState> chessRule_;
        std::string allyColor_;

        ItemPlacementMap itemPlacements_;
        std::function<TileCoords(Position const &)> const positionToTileConverter_;
        std::function<Position(TileCoords const &)> const tileToPositionConverter_;

    public:
        ChessRuleAdapter(
            std::shared_ptr<ItemStore> itemStore,
            std::shared_ptr<ChessBoardState> chessRule,
            std::string allyColor) noexcept;

        static auto getPositionToTileConverter(std::string color) noexcept
            -> std::function<TileCoords(Position const &)>;
        static auto getTileToPositionConverter(std::string color) noexcept
            -> std::function<Position(TileCoords const &)>;

    private:
        [[nodiscard]] auto positionToTile(
            Position const &position) const noexcept -> TileCoords override;
        [[nodiscard]] auto tileToPosition(
            TileCoords const &tile) const noexcept -> Position override;

        [[nodiscard]] auto chessMoveActionToItemMoveAction(
            ChessMove::Action const &chessMoveAction) const noexcept
            -> ChessItemMoveAction;
        [[nodiscard]] auto itemMoveActionToChessMoveAction(
            ChessItemMoveAction const &itemMoveAction) const noexcept
            -> ChessMove::Action;

        auto tryMove(
            ChessItemMove const &itemMove, std::string const &color) const noexcept
            -> ChessItemMoveAction override;

        void commitMove(
            ChessItemMoveAction const &itemMoveAction) noexcept override;

        auto getAllyColor() const -> std::string override;
        auto getEnemyColor() const -> std::string override;

        auto getItemColor(TileCoords const &tile) const noexcept
            -> std::optional<std::string> override;
        auto getItemType(TileCoords const &tile) const noexcept
            -> std::optional<std::string> override;

        auto getItemPlacements() -> ItemPlacementMap const & override;
        auto getSelectableTiles() const noexcept -> ItemPlacementMap override;
        auto getReachableTiles(TileCoords const &tile) const noexcept
            -> std::optional<ReachableTileInfo> override;

        auto getItemEntry(TileCoords const &tile) const noexcept
            -> ItemStore::Entry override;
    };
} // namespace bgg
