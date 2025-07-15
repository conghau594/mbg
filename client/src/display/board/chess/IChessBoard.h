// IChessBoard.h
#pragma once

#include <memory>
#include <limits>

#include <SFML/Graphics/Drawable.hpp>
#include "display/IGameBoard.h"
namespace sf
{
  class Event;
} // namespace sf

namespace bgg
{
  class IBoardState;
  class ChessMove;
  class IChessBoard : public IGameBoard
  {
  public:
    static sf::Vector2i constexpr FARTHEST_POSITION =
        {std::numeric_limits<int>::min(), std::numeric_limits<int>::min()};

    //virtual void requestMove(ChessMove const &move) = 0;

    virtual void changeState(
        std::shared_ptr<IBoardState> newState,
        sf::Vector2i const &mousePos = FARTHEST_POSITION) = 0;
    virtual void pushState(
        std::shared_ptr<IBoardState> newState,
        sf::Vector2i const &mousePos = FARTHEST_POSITION) = 0;
    virtual void popState(sf::Vector2i const &mousePos = FARTHEST_POSITION) = 0;
    virtual void clearStates() = 0;
  };
} // namespace bgg
