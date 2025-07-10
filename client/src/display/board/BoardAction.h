// BoardAction.h
#pragma once

#include <SFML/System/Vector2.hpp>
#include "peeb/Event.hpp"

namespace bgg
{
  class PieceMoveAction final
  {
  public:
    sf::Vector2i fromSquare;
    sf::Vector2i toSquare;
  };

  class ResignAction final
  {
  public:
    //int gameType;
  };

  using BoardAction = peeb::Event<
      PieceMoveAction,
      ResignAction>;

}