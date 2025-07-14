// IChessBoard.h
#pragma once

#include <memory>
#include <SFML/Graphics/Drawable.hpp>
#include "display/IGameBoard.h"

#include "MoveChessPiece.h"

namespace sf
{
  class Event;
} // namespace sf

namespace bgg
{
  class IBoardState;
  class IChessBoard : public IGameBoard
  {
  public:
    virtual void requestMove(MoveChessPiece const &move) = 0;

    virtual void changeState(std::shared_ptr<IBoardState> newState, sf::Vector2i const &mousePos) = 0;
    virtual void pushState(std::shared_ptr<IBoardState> newState, sf::Vector2i const &mousePos) = 0;
    virtual void popState(sf::Vector2i const &mousePos) = 0;
    virtual void clearStates() = 0;

  };
} // namespace bgg
