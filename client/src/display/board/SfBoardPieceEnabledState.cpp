// SfBoardPieceEnabledState.cpp

#include "SfBoardPieceEnabledState.h"

namespace bgg
{
  SfBoardPieceEnabledState::SfBoardPieceEnabledState(std::shared_ptr<SfGameBoard> gameBoard) noexcept
      : gameBoard_(gameBoard)
  {
  }

  void SfBoardPieceEnabledState::onEnter() noexcept
  {
  }

  void SfBoardPieceEnabledState::onExit() noexcept
  {
  }

  void SfBoardPieceEnabledState::onMouseMoved(sf::Vector2i const &mousePos) noexcept
  {
  }

  void SfBoardPieceEnabledState::onMousePressed(sf::Vector2i const &mousePos) noexcept
  {
    // gameBoard_->changeMouseState(nextMousePressedState_);
  }

  void SfBoardPieceEnabledState::onMouseReleased(sf::Vector2i const &mousePos) noexcept
  {
  }
} // namespace bgg
