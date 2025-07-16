// ChessMove.h
#pragma once

#include <optional>
#include <string>

#include <SFML/System/Vector2.hpp>

#include "base/Variant.h"

namespace bgg
{
  using TileCoords = sf::Vector2i;

  class ItemMove
  {
  public:
    TileCoords fromTile;
    TileCoords toTile;
    std::optional<std::string> promote;

    /////////////////////////////////////////////////////////////////////////////
    class Invalid
    {
    };

    class Normal
    {
    public:
    };

    class Promotion
    {
    public:
    };

    class EnPassant
    {
    public:
    };

    class Castling
    {
    public:
    };

    using Result = Variant<
        Invalid,
        Normal,
        Promotion,
        EnPassant,
        Castling>;
  };

} // namespace bgg
