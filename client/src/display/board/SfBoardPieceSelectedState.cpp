// SfBoardPieceSelectedState.cpp

#include "SfBoardPieceSelectedState.h"

namespace bgg
{
  SfBoardPieceSelectedState::SfBoardPieceSelectedState(
      std::shared_ptr<SfGameBoard> gameBoard,
      std::shared_ptr<SfGameRuleAdapter> gameRule,
      std::shared_ptr<SfTileMap> tileMap,
      std::shared_ptr<SfItemStore> itemStore) noexcept
      : gameBoard_(std::move(gameBoard)),
        gameRule_(std::move(gameRule)),
        tileMap_(std::move(tileMap)),
        itemStore_(std::move(itemStore))
  {
  }

  void SfBoardPieceSelectedState::onEnter() noexcept
  {
  }

  void SfBoardPieceSelectedState::onExit() noexcept
  {
  }

  void SfBoardPieceSelectedState::onMouseMoved(sf::Vector2i const &mousePos) noexcept
  {
  }

  void SfBoardPieceSelectedState::onMousePressed(sf::Vector2i const &mousePos) noexcept
  {
    // gameBoard_->changeMouseState(nextMousePressedState_);
  }

  void SfBoardPieceSelectedState::onMouseReleased(sf::Vector2i const &mousePos) noexcept
  {
  }
} // namespace bgg
