// SfGameRuleAdapter.h
#pragma once

#include <list>
#include <vector>
#include <optional>

#include <SFML/System/Vector2.hpp>

#include "SfItemStore.h"

namespace bgg
{
  using TileCoords = sf::Vector2i;

  class SfItemPlacement
  {
  public:
    SfItemStore::Entry entry;
    TileCoords tile;
  };

  class SfReachableTileInfo
  {
  public:
    SfItemPlacement itemPlacement;

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
    virtual auto getItemPlacements() const -> std::list<SfItemPlacement> = 0;

    [[nodiscard]]
    virtual auto getSelectableTiles() const -> std::list<SfItemPlacement> = 0;

    [[nodiscard]]
    virtual auto getReachableTiles(TileCoords const &tile) const
        -> std::optional<SfReachableTileInfo> = 0;

    [[nodiscard]]
    virtual auto getItemIndex(TileCoords const &tile) const -> int = 0;

    [[nodiscard]]
    virtual auto getItemEntry(int itemIndex) const -> SfItemStore::Entry = 0;

    /**
     * \return {-1, -1} if the item with itemIndex is not existing 
     */
    [[nodiscard]]
    virtual auto getItemTile(int itemIndex) const -> TileCoords = 0;
  };
} // namespace bgg
