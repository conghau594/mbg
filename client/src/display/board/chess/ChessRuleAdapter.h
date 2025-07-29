// ChessRuleAdapter.h
#pragma once

#include <vector>
#include <functional>

#include "IChessRuleAdapter.h"
#include "display/board/ItemStore.h"
#include "model/chess/IChessRule.h"

#define BGG_VALIDATE_TILE(tile) BOOST_ASSERT_MSG(             \
    tile.x >= 0 && tile.x <= 7 && tile.y >= 0 && tile.y <= 7, \
    "Tile coords must be from 0 to 7")

namespace bgg
{
    class BoardItem;

    class ChessRuleAdapter final : public IChessRuleAdapter
    {
        std::shared_ptr<IChessRule> chessRule_;
        Side allyColor_;
        Side opponentColor_;

        ItemPlacementMap itemPlacements_;
        std::function<TileCoords(Position const &)> const posToTileConverter_;
        std::function<Position(TileCoords const &)> const tileToPositionConverter_;

    public:
        ChessRuleAdapter(
            std::shared_ptr<ItemStore> itemStore,
            std::shared_ptr<IChessRule> chessRule,
            Side const &allyColor) noexcept;

    private:
        static auto getPositionToTileConverter(Side const &color) noexcept
            -> std::function<TileCoords(Position const &)>;
        static auto getTileToPositionConverter(Side const &color) noexcept
            -> std::function<Position(TileCoords const &)>;

        [[nodiscard]] auto positionToTile(
            Position const &position) const noexcept -> TileCoords override;
        [[nodiscard]] auto tileToPosition(
            TileCoords const &tile) const noexcept -> Position override;

        // [[nodiscard]] auto chessMoveActionToItemMoveAction(
        //     ChessMove::Detail const &chessMoveAction) const noexcept
        //     -> ChessItemMoveAction;
        // [[nodiscard]] auto itemMoveActionToChessMoveAction(
        //     ChessItemMoveAction const &itemMoveAction) const noexcept
        //     -> ChessMove::Detail;

        auto getAllyColor() const -> Side override;
        auto getOpponentColor() const -> Side override;

        auto getItemInfo(TileCoords const &tile) const noexcept
            -> std::optional<ItemInfo> override;

        auto getItemPlacements() -> ItemPlacementMap const & override;
        auto collectSelectableTiles() const noexcept -> ItemPlacementMap override;
        auto collectReachableTiles(TileCoords const &tile) const noexcept
            -> std::optional<ReachableTileInfo> override;

        auto getItemEntry(TileCoords const &tile) const noexcept
            -> ItemStore::Entry override;

        auto tryMove(
            ChessItemMove const &itemMove, Side const &color) const noexcept
            -> ChessItemMove::Detail override;

        void commitMove(
            ChessItemMove::Detail const &itemMoveDetail) noexcept override;

        void commitMove(
            ChessMove::Detail const &nativeMoveDetail) noexcept override;

        // auto chessMoveToItemMove(ChessMove::Detail) const
        //     -> ChessItemMove::Detail override;
    };
} // namespace bgg
