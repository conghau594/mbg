// SfBoardPieceDisabledState.cpp

#include "SfBoardPieceDisabledState.h"

namespace bgg
{
  SfBoardPieceDisabledState::SfBoardPieceDisabledState(
      std::shared_ptr<SfGameBoard> gameBoard) noexcept
      : gameBoard_(gameBoard)
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
