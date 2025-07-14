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
    sf::Vector2i fromTile;
    sf::Vector2i toTile;
    std::optional<std::string> promote;
  };

}