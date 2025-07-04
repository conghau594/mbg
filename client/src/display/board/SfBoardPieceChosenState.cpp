// SfBoardPieceChosenState.cpp

#include "SfBoardPieceChosenState.h"

namespace bgg
{
  SfBoardPieceChosenState::SfBoardPieceChosenState(
      std::shared_ptr<SfGameBoard> gameBoard) noexcept
      : gameBoard_(std::move(gameBoard))
  {
  }

  void SfBoardPieceChosenState::onEnter() noexcept
  {
  }

  void SfBoardPieceChosenState::onExit() noexcept
  {
  }

  void SfBoardPieceChosenState::onMouseMoved(sf::Vector2i const &mousePos) noexcept
  {
  }

  void SfBoardPieceChosenState::onMousePressed(sf::Vector2i const &mousePos) noexcept
  {
    // gameBoard_->changeMouseState(nextMousePressedState_);
  }

  void SfBoardPieceChosenState::onMouseReleased(sf::Vector2i const &mousePos) noexcept
  {
  }
} // namespace bgg
