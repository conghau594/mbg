// BoardAction.h
#pragma once

#include <optional>

#include <SFML/System/Vector2.hpp>
#include "peeb/Event.hpp"

namespace bgg
{
  class MoveChessPiece final
  {
  public:
    sf::Vector2i fromSquare;
    sf::Vector2i toSquare;
    std::optional<int> promotedPiece;
  };

  class DrawCard final
  {
  public:
    // int gameType;
  };

  using BoardAction = peeb::Event<
      MoveChessPiece,
      DrawCard>;

}