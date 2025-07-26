// ChessPieceSelectableState.h
#pragma once

#include <memory>
#include <map>

#include "display/board/IBoardViewState.h"
#include "display/board/ItemStore.h"
#include "IChessRuleAdapter.h"

namespace bgg
{
  class IChessBoardView;
  class TileMap;

  class ChessPieceSelectableState final : public IBoardViewState
  {
    std::shared_ptr<IChessBoardView> gameBoard_;

    std::shared_ptr<IChessRuleAdapter> gameRule_;
    std::shared_ptr<TileMap> tileMap_;
    std::shared_ptr<ItemStore> itemStore_;

    ItemPlacementMap selectableTiles_;
    ItemStore::Entry choiceHighlighter_;
    TileCoords lastHoveredTile_;

  public:
    ChessPieceSelectableState(
        std::shared_ptr<IChessBoardView> gameBoard,
        std::shared_ptr<IChessRuleAdapter> gameRule,
        std::shared_ptr<TileMap> tileMap,
        std::shared_ptr<ItemStore> itemStore) noexcept;

    ~ChessPieceSelectableState() noexcept;

  private:
    void onEnter(sf::Vector2i const &mousePos) noexcept override;
    void onExit() noexcept override;
    void onMouseMoved(sf::Vector2i const &mousePos) noexcept override;
    void onMousePressed(sf::Vector2i const &mousePos) noexcept override;
    void onMouseReleased(sf::Vector2i const &mousePos) noexcept override;
    void onServerMessage(ServerMessage const &msg) noexcept override;
  };
}