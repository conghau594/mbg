// IChessBoardView.h
#pragma once

#include <memory>
#include <limits>

#include <SFML/Graphics/Drawable.hpp>
#include "display/IBoardView.h"
namespace sf
{
  class Event;
} // namespace sf

namespace bgg
{
  class IBoardViewState;
  class MoveRequest;
  class IChessBoardView : public IBoardView
  {
  public:
    static sf::Vector2i constexpr FARTHEST_POSITION =
        {std::numeric_limits<int>::min(), std::numeric_limits<int>::min()};

    virtual void sendMoveRequest(MoveRequest const &move) = 0;

    virtual void changeState(
        std::shared_ptr<IBoardViewState> newState,
        sf::Vector2i const &mousePos = FARTHEST_POSITION) = 0;
    virtual void pushState(
        std::shared_ptr<IBoardViewState> newState,
        sf::Vector2i const &mousePos = FARTHEST_POSITION) = 0;
    virtual void popState(sf::Vector2i const &mousePos = FARTHEST_POSITION) = 0;
    virtual void clearStates() = 0;
  };
} // namespace bgg
