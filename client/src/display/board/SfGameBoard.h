// SfGameBoard.h
#pragma once

#include <memory>
#include <SFML/Graphics/Drawable.hpp>
#include "service/ClientEvent.h"
#include "SfItemStore.h"

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
    virtual void changeState(std::shared_ptr<SfBoardState> newState) = 0;
    virtual void pushState(std::shared_ptr<SfBoardState> newState) = 0;
    virtual void popState() = 0;
    virtual void clearStates() = 0;

    virtual void send(ClientEvent const &request) = 0;
  };
} // namespace bgg
