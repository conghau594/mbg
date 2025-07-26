// IBoardViewState.h
#pragma once

#include <SFML/System/Vector2.hpp>

#include "service/ServerMessage.h"

namespace bgg
{
  class IBoardViewState
  {
  public:
    virtual ~IBoardViewState() = default;
    virtual void onEnter(sf::Vector2i const &mousePos) = 0;
    virtual void onExit() = 0;
    virtual void onMouseMoved(sf::Vector2i const &mousePos) = 0;
    virtual void onMousePressed(sf::Vector2i const &mousePos) = 0;
    virtual void onMouseReleased(sf::Vector2i const &mousePos) = 0;
    virtual void onServerMessage(ServerMessage const &msg) = 0;
  };
} // namespace bgg
