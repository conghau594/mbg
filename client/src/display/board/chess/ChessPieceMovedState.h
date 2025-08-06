// ChessPieceMovedState.h
#pragma once

#include <memory>

#include "ChessItemMove.h"

#include "display/board/ItemStore.h"
#include "display/board/BoardDisabledState.h"

namespace bgg
{
  class IChessBoardView;
  class IChessRuleAdapter;
  class TileMap;

  class ChessPieceMovedState final : public BoardDisabledState
  {
    std::shared_ptr<IChessBoardView> gameBoard_;

    std::shared_ptr<IChessRuleAdapter> gameRule_;
    std::shared_ptr<TileMap> tileMap_;
    std::shared_ptr<ItemStore> itemStore_;

    ItemStore::Entry pendingPromotionItem_;
    ItemStore::Entry pendingMoveHighlighters_[2];
    ItemStore::Entry opponentCheckHighlighter_;

    BoardItem *lastMoveHighlighters_[2];
    bool lastMoveHighlighterVisibility_[2]; ///< the original visibility of lastMoveHighlighters_

    BoardItem *allyCheckHighlighter_;
    bool allyCheckHighlighterVisibility_; ///< the original visibility of allyCheckHighlighter_

    std::optional<ChessMoveDetailAdapter> pendingMoveDetail_;

  public:
    ChessPieceMovedState(
        std::shared_ptr<IChessBoardView> gameBoard,
        std::shared_ptr<IChessRuleAdapter> gameRule,
        std::shared_ptr<TileMap> tileMap,
        std::shared_ptr<ItemStore> itemStore,
        ChessItemMove const &itemMove) noexcept;

  private:
    void onEnter(sf::Vector2i const &mousePos) noexcept override;
    void onServerMessage(ServerMessage const &msg) noexcept override;

    void initHighlighters() noexcept;
    void previewMoveAction() noexcept;
    void revertMoveAction() noexcept;
    void finalizeMoveAction() noexcept;
  };
} // namespace bgg
