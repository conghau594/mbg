// SfGameBoard.h
#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include "service/ClientEvent.h"

namespace std
{
  template <typename T>
  class shared_ptr;
}

namespace sf
{
  class Event;
} // namespace sf

namespace bgg
{
  class SfBoardState;
  class SfGameBoard : public sf::Drawable
  {
  public:
    virtual void onEvent(sf::Event const &event) = 0;
    virtual void send(ClientEvent const &request) = 0;
    virtual void changeMouseState(std::shared_ptr<SfBoardState> newMouseState) = 0;
  };
} // namespace bgg
