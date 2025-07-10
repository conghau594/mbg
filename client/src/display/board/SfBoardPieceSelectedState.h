// SfBoardPieceSelectedState.h
#pragma once

#include <memory>

#include "SfBoardState.h"
#include "SfItemStore.h"
#include "SfGameRuleAdapter.h"

namespace bgg
{
  class SfGameBoard;
  class SfTileMap;

  class SfBoardPieceSelectedState final : public SfBoardState
  {
    std::shared_ptr<SfGameBoard> gameBoard_;

    std::shared_ptr<SfGameRuleAdapter> gameRule_;
    std::shared_ptr<SfTileMap> tileMap_;
    std::shared_ptr<SfItemStore> itemStore_;

    SfItemPlacementMap reachableTiles_;

    std::list<SfItemStore::Entry> staticHighlighters_; ///< 'static' means these highlighters do not change until this state exits

    SfItemStore::Entry choiceHighlighter_; ///< choiceHighlighter_ is a dynamic highlighter
    SfItemStore::Entry selectedItemEntry_;
    int originalSelectedItemZOrder_;

    TileCoords selectedTile_;
    TileCoords lastHoveredTile_;

  public:
    SfBoardPieceSelectedState(
        std::shared_ptr<SfGameBoard> gameBoard,
        std::shared_ptr<SfGameRuleAdapter> gameRule,
        std::shared_ptr<SfTileMap> tileMap,
        std::shared_ptr<SfItemStore> itemStore,
        TileCoords selectedTile) noexcept;

    ~SfBoardPieceSelectedState();

  private:
    void onEnter() noexcept override;
    void onExit() noexcept override;
    void onMouseMoved(sf::Vector2i const &mousePos) noexcept override;
    void onMousePressed(sf::Vector2i const &mousePos) noexcept override;
    void onMouseReleased(sf::Vector2i const &mousePos) noexcept override;

    void addHighlighters(SfReachableTileInfo &reachableTileInfo) noexcept;
  };
}