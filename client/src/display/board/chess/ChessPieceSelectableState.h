// ChessPieceSelectableState.h
#pragma once

#include <memory>
#include <map>

#include "display/board/IBoardState.h"
#include "display/board/ItemStore.h"
#include "IChessRuleAdapter.h"

namespace bgg
{
  class IChessBoard;
  class TileMap;

  class ChessPieceSelectableState final : public IBoardState
  {
    std::shared_ptr<IChessBoard> gameBoard_;

    std::shared_ptr<IChessRuleAdapter> gameRule_;
    std::shared_ptr<TileMap> tileMap_;
    std::shared_ptr<ItemStore> itemStore_;

    ItemPlacementMap selectableTiles_;
    ItemStore::Entry choiceHighlighter_;
    TileCoords lastHoveredTile_;

  public:
    ChessPieceSelectableState(
        std::shared_ptr<IChessBoard> gameBoard,
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