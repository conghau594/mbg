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
        SfItemPlacementMap itemPlacements_; ///< Usage: itemEntries_[ChessPiece::<ENUM>]
        ChessRule rule_;
        TileCoords (*const squareToTileConverter_)(ChessRule::Square const &);
        std::functional<ChessRule::Square(TileCoords const &)> const tileToSquareConverter_;

    public:
        SfChessRuleAdapter(ChessRule rule) noexcept;

        static constexpr auto getSquareToTileConverter(ChessPiece::Color color) noexcept
            -> std::functional<TileCoords(ChessRule::Square const &)>;
        static constexpr auto getTileToSquareConverter(ChessPiece::Color color) noexcept
            -> std::functional<ChessRule::Square(TileCoords const &)>;

    private:
        auto getSide() const -> int override;

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
