// BoardDisabledState.h
#pragma once

#include "IBoardViewState.h"

namespace bgg
{
  class BoardDisabledState final : public IBoardViewState
  {
  protected:
    void onEnter(sf::Vector2i const &mousePos) noexcept override {}
    void onExit() noexcept override {}
    void onMouseMoved(sf::Vector2i const &mousePos) noexcept override {}
    void onMousePressed(sf::Vector2i const &mousePos) noexcept override {}
    void onMouseReleased(sf::Vector2i const &mousePos) noexcept override {}
    void onServerMessage(ServerMessage const &msg) noexcept override {}
  };
}