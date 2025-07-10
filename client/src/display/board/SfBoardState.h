// SfBoardState.h
#pragma once

#include <SFML/System/Vector2.hpp>

namespace bgg
{
  class SfBoardState
  {
  public:
    virtual ~SfBoardState() = default;
    virtual void onEnter(sf::Vector2i const &mousePos) = 0;
    virtual void onExit() = 0;
    virtual void onMouseMoved(sf::Vector2i const &mousePos) = 0;
    virtual void onMousePressed(sf::Vector2i const &mousePos) = 0;
    virtual void onMouseReleased(sf::Vector2i const &mousePos) = 0;
  };
} // namespace bgg
