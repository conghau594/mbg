// SfBoardPieceDisabledState.cpp

#include "SfBoardPieceDisabledState.h"

namespace bgg
{
  SfBoardPieceDisabledState::SfBoardPieceDisabledState(
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

  void SfBoardPieceDisabledState::onEnter() noexcept
  {
  }

  void SfBoardPieceDisabledState::onExit() noexcept
  {
  }

  void SfBoardPieceDisabledState::onMouseMoved(sf::Vector2i const &mousePos) noexcept
  {
  }

  void SfBoardPieceDisabledState::onMousePressed(sf::Vector2i const &mousePos) noexcept
  {
    // gameBoard_->changeMouseState(nextMousePressedState_);
  }

  void SfBoardPieceDisabledState::onMouseReleased(sf::Vector2i const &mousePos) noexcept
  {
  }
} // namespace bgg
