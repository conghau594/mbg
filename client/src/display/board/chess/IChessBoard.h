// IChessBoard.h
#pragma once

#include <memory>
#include <SFML/Graphics/Drawable.hpp>
#include "display/IGameBoard.h"

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
    virtual void requestMove(
        sf::Vector2i fromTile,
        sf::Vector2i toTile,
        std::optional<std::string> promote) = 0;

    virtual void changeState(std::shared_ptr<IBoardState> newState, sf::Vector2i const &mousePos) = 0;
    virtual void pushState(std::shared_ptr<IBoardState> newState, sf::Vector2i const &mousePos) = 0;
    virtual void popState(sf::Vector2i const &mousePos) = 0;
    virtual void clearStates() = 0;
  };
} // namespace bgg
