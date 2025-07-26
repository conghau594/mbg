// ChessPieceMovedState.h
#pragma once

#include <memory>

#include "display/board/IBoardViewState.h"
#include "ChessItemMove.h"
#include "display/board/ItemStore.h"

namespace bgg
{
  class IChessBoardView;
  class IChessRuleAdapter;
  class TileMap;
  class ItemMove;

  class ChessPieceMovedState final : public IBoardViewState
  {
    std::shared_ptr<IChessBoardView> gameBoard_;

    std::shared_ptr<IChessRuleAdapter> gameRule_;
    std::shared_ptr<TileMap> tileMap_;
    std::shared_ptr<ItemStore> itemStore_;

    ItemStore::Entry pendingPromotionItem_;
    ItemStore::Entry pendingMoveHighlighters_[2];
    ItemStore::Entry enemyCheckHighlighter_;

    BoardItem *lastMoveHighlighters_[2];
    bool lastMoveHighlighterVisibility_[2];

    BoardItem *allyCheckHighlighter_;
    bool allyCheckHighlighterVisibility_;

    ChessItemMoveAction pendingItemMoveAction_;

  public:
    ChessPieceMovedState(
        std::shared_ptr<IChessBoardView> gameBoard,
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

    void revertMoveAction() noexcept;
    void finalizeMoveAction() noexcept;

    void finalizeBasicMoveAction(
        TileCoords const &fromTile, TileCoords const &toTile) noexcept;

    void onGameUpdatedNotification(
        GameUpdatedNotification const &notif) noexcept;
  };
} // namespace bgg
