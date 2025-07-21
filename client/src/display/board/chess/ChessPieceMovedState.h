// ChessPieceMovedState.h
#pragma once

#include <memory>

#include "display/board/IBoardState.h"
#include "ChessItemMove.h"
#include "display/board/ItemStore.h"

namespace bgg
{
  class IChessBoard;
  class IChessRuleAdapter;
  class TileMap;
  class ItemMove;

  class ChessPieceMovedState final : public IBoardState
  {
    std::shared_ptr<IChessBoard> gameBoard_;

    std::shared_ptr<IChessRuleAdapter> gameRule_;
    std::shared_ptr<TileMap> tileMap_;
    std::shared_ptr<ItemStore> itemStore_;

    ItemStore::Entry checkHighlighter_;
    ItemStore::Entry pendingPromotionItem_;
    ItemStore::Entry pendingMoveHighlighters_[2];

    BoardItem *lastMoveHighlighters_[2];
    bool lastMoveHighlighterVisibility_[2];

    ChessItemMoveAction pendingItemMoveAction_;

  public:
    ChessPieceMovedState(
        std::shared_ptr<IChessBoard> gameBoard,
        std::shared_ptr<IChessRuleAdapter> gameRule,
        std::shared_ptr<TileMap> tileMap,
        std::shared_ptr<ItemStore> itemStore,
        std::optional<ItemMove> const &move) noexcept;

  private:
    void onEnter(sf::Vector2i const &mousePos) noexcept override;
    void onExit() noexcept override;
    void onMouseMoved(sf::Vector2i const &mousePos) noexcept override;
    void onMousePressed(sf::Vector2i const &mousePos) noexcept override;
    void onMouseReleased(sf::Vector2i const &mousePos) noexcept override;
    void onServerMessage(ServerMessage const &msg) noexcept override;

    void updateBoard(ChessItemMoveAction const &itemMoveAction) noexcept;

    void previewBasicMoveAction(
        BoardItem &movedItemEntry,
        TileCoords const &fromTile,
        TileCoords const &toTile,
        TileCoords const &enemyKingTile,
        KingState const &enemyKingState) noexcept;

    void onMoveResponse(MoveResponse const &response) noexcept;
    void revertBasicMoveAction(
        BoardItem &movedItemEntry,
        TileCoords const &fromTile,
        TileCoords const &toTile) noexcept;

    void finalizeBasicMoveAction(
        TileCoords const &fromTile,
        TileCoords const &toTile) noexcept;
  };
} // namespace bgg
