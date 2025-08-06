// IChessRuleAdapter.h
#pragma once

#include <map>
#include <list>
#include <vector>
#include <optional>

#include "display/board/ItemStore.h"
#include "model/Piece.h"
#include "model/chess/ChessMove.h"
#include "ChessItemMove.h"

namespace bgg
{
  class TileComparator
  {
  public:
    [[nodiscard]] auto operator()(
        const TileCoords &lhs, const TileCoords &rhs) const noexcept -> bool
    {
      if (lhs.x == rhs.x)
      {
        return lhs.y < rhs.y;
      }

      return lhs.x < rhs.x;
    }
  };

  using ItemPlacementMap = std::map<TileCoords, ItemStore::Entry, TileComparator>;

  class ReachableTileInfo
  {
  public:
    ItemStore::Entry entry;

    std::list<TileCoords> quietSquares;
    std::list<TileCoords> captureSquares;
    std::list<TileCoords> specialMoveTiles;

    // std::optional<TileCoords> checkMove;
    // bool isCheckmate;
  };

  class IChessRuleAdapter
  {
  public:
    virtual ~IChessRuleAdapter() = default;

    [[nodiscard]]
    virtual auto getAllyColor() const -> Side = 0;

    [[nodiscard]]
    virtual auto getOpponentColor() const -> Side = 0;

    [[nodiscard]]
    virtual auto getItemInfo(TileCoords const &tile) const
        -> std::optional<ItemInfo> = 0;

    [[nodiscard]]
    virtual auto getItemPlacements() -> ItemPlacementMap const & = 0;

    [[nodiscard]]
    virtual auto collectSelectableTiles() const -> ItemPlacementMap = 0;

    [[nodiscard]]
    virtual auto collectReachableTiles(TileCoords const &tile) const
        -> std::optional<ReachableTileInfo> = 0;

    [[nodiscard]]
    virtual auto getItemEntry(TileCoords const &tile) const
        -> ItemStore::Entry = 0;

    [[nodiscard]]
    virtual auto positionToTile(Position const &position) const -> TileCoords = 0;
    [[nodiscard]]
    virtual auto tileToPosition(TileCoords const &tile) const -> Position = 0;

    [[nodiscard]]
    virtual auto tryMove(ChessItemMove const &itemMove) const
        -> ChessMoveDetailAdapter = 0;

    /**
     * \return number of the move
     */
    virtual auto commitMove(ChessMoveDetailAdapter const &itemMoveDetail) noexcept
        -> int = 0;

    virtual auto commitMove(ChessMove::Detail const &nativeMoveDetail) noexcept
        -> int = 0;

    // [[nodiscard]]
    // virtual auto chessMoveToItemMove(ChessMove::Detail) const
    //     -> ChessMoveDetailAdapter = 0;
    // virtual auto addItemEntry(TileCoords tile, ItemStore::Entry entry) -> bool = 0;
  };
} // namespace bgg
