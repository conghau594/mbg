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
  class SfItem;
  class SfGameBoard : public sf::Drawable
  {
  public:
    virtual void onEvent(sf::Event const &event) = 0;
    virtual void send(ClientEvent const &request) = 0;
    virtual void changeState(std::shared_ptr<SfBoardState> newState) = 0;
    virtual auto addItem(
        sf::Vector2i tileCoords,
        size_t zOrder,
        int textureCellIndex,
        bool visible = true) -> SfItem * = 0;

    virtual auto removeItem(SfItem *item) -> bool = 0;
    virtual auto putItemOnTile(SfItem *item, sf::Vector2i tileCoords) -> bool = 0;
  };
} // namespace bgg
