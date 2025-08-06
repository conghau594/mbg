// ChessPieceDisabledState.h
#pragma once

#include <memory>

#include "display/board/ItemStore.h"
#include "display/board/BoardDisabledState.h"

namespace bgg
{
  class IChessBoardView;
  class IChessRuleAdapter;
  class TileMap;

  class ChessPieceDisabledState final : public BoardDisabledState
  {
    std::shared_ptr<IChessBoardView> gameBoard_;

    std::shared_ptr<IChessRuleAdapter> gameRule_;
    std::shared_ptr<TileMap> tileMap_;
    std::shared_ptr<ItemStore> itemStore_;

    BoardItem *lastMoveHighlighters_[2];
    // bool lastMoveHighlighterVisibility_[2]; ///< the lastMoveHighlighters_ always exists

    BoardItem *checkHighlighter_;
    // bool checkHighlighterVisibility_;

  public:
    ChessPieceDisabledState(
        std::shared_ptr<IChessBoardView> gameBoard,
        std::shared_ptr<IChessRuleAdapter> gameRule,
        std::shared_ptr<TileMap> tileMap,
        std::shared_ptr<ItemStore> itemStore) noexcept;

  private:
    void onServerMessage(ServerMessage const &msg) noexcept override;
    void onEnter(sf::Vector2i const &mousePos) noexcept override;

    void initHighlighters() noexcept;
    void updatedOpponentMove(
        ChessMove::Detail const &nativeMoveDetail) noexcept;
  };
} // namespace bgg