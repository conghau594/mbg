// ChessPieceSelectedState.h
#pragma once

#include <memory>

#include "display/board/IBoardState.h"
#include "display/board/ItemStore.h"
#include "IChessRuleAdapter.h"

namespace bgg
{
  class IChessBoard;
  class TileMap;

  class ChessPieceSelectedState final : public IBoardState
  {
    std::shared_ptr<IChessBoard> gameBoard_;

    std::shared_ptr<IChessRuleAdapter> gameRule_;
    std::shared_ptr<TileMap> tileMap_;
    std::shared_ptr<ItemStore> itemStore_;

    ItemPlacementMap reachableTiles_;

    std::list<ItemStore::Entry> staticHighlighters_; ///< 'static' means these highlighters do not change until this state exits

    ItemStore::Entry choiceHighlighter_; ///< choiceHighlighter_ is a dynamic highlighter
    ItemStore::Entry selectedItemEntry_;
    int originalSelectedItemZOrder_;

    TileCoords selectedTile_;
    TileCoords lastHoveredTile_;

  public:
    ChessPieceSelectedState(
        std::shared_ptr<IChessBoard> gameBoard,
        std::shared_ptr<IChessRuleAdapter> gameRule,
        std::shared_ptr<TileMap> tileMap,
        std::shared_ptr<ItemStore> itemStore,
        TileCoords selectedTile) noexcept;

    ~ChessPieceSelectedState();

  private:
    void onEnter(sf::Vector2i const &mousePos) noexcept override;
    void onExit() noexcept override;
    void onMouseMoved(sf::Vector2i const &mousePos) noexcept override;
    void onMousePressed(sf::Vector2i const &mousePos) noexcept override;
    void onMouseReleased(sf::Vector2i const &mousePos) noexcept override;

    void addHighlighters(ReachableTileInfo &reachableTileInfo) noexcept;
  };
}