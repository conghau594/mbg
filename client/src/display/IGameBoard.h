// IGameBoard.h
#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include "service/ServerMessage.h"

namespace sf
{
  class Event;
} // namespace sf

namespace bgg
{
  class IGameBoard : public sf::Drawable
  {
  public:
    virtual void onWindowEvent(sf::Event const &event) = 0;
    virtual void handleServerMessage(ServerMessage const& msg) = 0;

  };
} // namespace bgg
