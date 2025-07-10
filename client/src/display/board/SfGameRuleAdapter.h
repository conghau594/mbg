// SfGameRuleAdapter.h
#pragma once

#include <map>
#include <list>
#include <vector>
#include <optional>

#include <SFML/System/Vector2.hpp>

#include "SfItemStore.h"

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

  using SfItemPlacementMap = std::map<TileCoords, SfItemStore::Entry, TileComparator>;

  class SfReachableTileInfo
  {
  public:
    SfItemStore::Entry entry;
    TileCoords tile;

    std::list<TileCoords> quietMoves;
    std::list<TileCoords> captureMoves;
    std::vector<TileCoords> specialMoves;

    // std::optional<TileCoords> checkMove;
    // bool isCheckmate;
  };

  class SfGameRuleAdapter
  {
  public:
    virtual ~SfGameRuleAdapter() = default;

    [[nodiscard]]
    virtual auto getSide() const -> int = 0;

    [[nodiscard]]
    virtual auto getItemPlacements() const -> SfItemPlacementMap = 0;

    [[nodiscard]]
    virtual auto getSelectableTiles() const -> SfItemPlacementMap = 0;

    [[nodiscard]]
    virtual auto getReachableTiles(TileCoords const &tile) const
        -> std::optional<SfReachableTileInfo> = 0;

    [[nodiscard]]
    virtual auto getItemIndex(TileCoords const &tile) const -> std::optional<int> = 0;

    [[nodiscard]]
    virtual auto getItemEntry(int itemIndex) const -> std::optional<SfItemStore::Entry> = 0;

    /**
     * \return {-1, -1} if the item with itemIndex is not existing
     */
    [[nodiscard]]
    virtual auto getItemTile(int itemIndex) const -> std::optional<TileCoords> = 0;
  };
} // namespace bgg
