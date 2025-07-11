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
    virtual auto getItemPlacements() -> SfItemPlacementMap& = 0;

    [[nodiscard]]
    virtual auto getSelectableTiles() const -> SfItemPlacementMap = 0;

    [[nodiscard]]
    virtual auto getReachableTiles(TileCoords const &tile) const
        -> std::optional<SfReachableTileInfo> = 0;

    // /**
    //  * \return type of piece at the `tile`
    //  */
    // [[nodiscard]]
    // virtual auto getItemType(TileCoords const &tile) const -> std::optional<int> = 0;

    [[nodiscard]]
    virtual auto getItemEntry(TileCoords const &tile) const -> std::optional<SfItemStore::Entry> = 0;
    };
} // namespace bgg
