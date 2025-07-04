// SfBoardPieceEnabledState.h
#pragma once

#include <memory>

#include "SfBoardState.h"

namespace bgg
{
  class SfGameBoard;
  class SfBoardPieceEnabledState final : public SfBoardState
  {
    std::shared_ptr<SfGameBoard> gameBoard_;

  public:
    SfBoardPieceEnabledState(std::shared_ptr<SfGameBoard> gameBoard) noexcept;

  private:
    void onEnter() noexcept override;
    void onExit() noexcept override;
    void onMouseMoved(sf::Vector2i const &mousePos) noexcept override;
    void onMousePressed(sf::Vector2i const &mousePos) noexcept override;
    void onMouseReleased(sf::Vector2i const &mousePos) noexcept override;
  };
}