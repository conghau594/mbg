// SfGameBoard.h
#pragma once

#include <memory>
#include <SFML/Graphics/Drawable.hpp>
#include "BoardAction.h"
#include "SfItemStore.h"
#include "service/ServerMessage.h"

namespace sf
{
  class Event;
} // namespace sf

namespace bgg
{
  class SfBoardState;
  class SfBoardItem;
  class SfGameBoard : public sf::Drawable
  {
  public:
    virtual void onEvent(sf::Event const &event) = 0;
    virtual void commitAction(BoardAction const &action) = 0;
    virtual void handleServerMessage(ServerMessage const& msg) = 0;

    virtual void changeState(std::shared_ptr<SfBoardState> newState, sf::Vector2i const &mousePos) = 0;
    virtual void pushState(std::shared_ptr<SfBoardState> newState, sf::Vector2i const &mousePos) = 0;
    virtual void popState(sf::Vector2i const &mousePos) = 0;
    virtual void clearStates() = 0;

  };
} // namespace bgg
