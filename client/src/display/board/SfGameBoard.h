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
  class SfBoardItem;
  class SfGameBoard : public sf::Drawable
  {
  public:
    virtual void onEvent(sf::Event const &event) = 0;
    virtual void send(ClientEvent const &request) = 0;
    virtual void changeState(std::shared_ptr<SfBoardState> newState) = 0;

    /**
     *
     * \return pair of id and pointer to the item in the board
     */
    virtual auto addItem(SfBoardItem item, int zOrder, bool visible = true)
        -> std::pair<std::size_t, SfBoardItem *> = 0;
    virtual auto addItem(
        sf::Vector2i const &tileCoords,
        sf::Vector2i const &textureCellIndex,
        int zOrder,
        std::string name = "",
        bool visible = true) -> std::pair<std::size_t, SfBoardItem *> = 0;

    /**
     * \return true if there is a item with `itemId` to remove
     */
    virtual auto removeItem(std::size_t itemId) -> bool = 0;
    virtual auto putItemOnTile(SfBoardItem *item, sf::Vector2i const &tileCoords) -> bool = 0;
  };
} // namespace bgg
