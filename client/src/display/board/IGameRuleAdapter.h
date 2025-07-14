// IGameRuleAdapter.h
#pragma once

#include <map>
#include <list>
#include <vector>
#include <optional>

#include <SFML/System/Vector2.hpp>

#include "ItemStore.h"
#include "model/Piece.h"

namespace bgg
{
  using TileCoords = sf::Vector2i;
  class TileComparator
  {
  public:
    auto operator()(
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

    std::list<TileCoords> quietMoves;
    std::list<TileCoords> captureMoves;
    std::vector<TileCoords> specialMoves;

    // std::optional<TileCoords> checkMove;
    // bool isCheckmate;
  };

  class IGameRuleAdapter
  {
  public:
    virtual ~IGameRuleAdapter() = default;

    [[nodiscard]]
    virtual auto getSide() const -> std::string const & = 0;

    [[nodiscard]]
    virtual auto getItemPlacements() -> ItemPlacementMap & = 0;

    [[nodiscard]]
    virtual auto getSelectableTiles() const -> ItemPlacementMap = 0;

    [[nodiscard]]
    virtual auto getReachableTiles(TileCoords const &tile) const
        -> std::optional<ReachableTileInfo> = 0;

    // /**
    //  * \return type of piece at the `tile`
    //  */
    // [[nodiscard]]
    // virtual auto getItemType(TileCoords const &tile) const -> std::optional<int> = 0;

    [[nodiscard]]
    virtual auto getItemEntry(TileCoords const &tile) const
        -> std::optional<ItemStore::Entry> = 0;

    [[nodiscard]]
    virtual auto positionToTile(Position const &position) const -> TileCoords = 0;
    [[nodiscard]]
    virtual auto tileToPosition(TileCoords const &tile) const -> Position = 0;
  };
} // namespace bgg
