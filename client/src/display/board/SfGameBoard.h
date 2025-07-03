// SfGameBoard.h
#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include "service/ClientEvent.h"

namespace sf
{
  class Event;
} // namespace sf

namespace bgg
{
  class SfGameBoard : public sf::Drawable
  {
  public:
    virtual void onEvent(sf::Event const &event) = 0;
    virtual void send(ClientEvent const &request) = 0;
  };
} // namespace bgg
