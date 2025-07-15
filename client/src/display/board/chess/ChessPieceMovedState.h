// ChessPieceMovedState.h
#pragma once

#include <memory>

#include "display/board/IBoardState.h"

namespace bgg
{
  class IChessBoard;
  class IChessRuleAdapter;
  class TileMap;
  class ItemStore;
  class ItemMove;

  class ChessPieceMovedState final : public IBoardState
  {
    std::shared_ptr<IChessBoard> gameBoard_;

    std::shared_ptr<IChessRuleAdapter> gameRule_;
    std::shared_ptr<TileMap> tileMap_;
    std::shared_ptr<ItemStore> itemStore_;


  public:
    ChessPieceMovedState(
        std::shared_ptr<IChessBoard> gameBoard,
        std::shared_ptr<IChessRuleAdapter> gameRule,
        std::shared_ptr<TileMap> tileMap,
        std::shared_ptr<ItemStore> itemStore,
        ItemMove const &move) noexcept;

  private:
    void onEnter(sf::Vector2i const &mousePos) noexcept override;
    void onExit() noexcept override;
    void onMouseMoved(sf::Vector2i const &mousePos) noexcept override;
    void onMousePressed(sf::Vector2i const &mousePos) noexcept override;
    void onMouseReleased(sf::Vector2i const &mousePos) noexcept override;
    void onServerMessage(ServerMessage const &msg) noexcept override;
  };
} // namespace bgg
