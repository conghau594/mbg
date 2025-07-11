// ChessPiece.h
#pragma once

#include <string>

#include <boost/assert.hpp>

namespace bgg
{
  class ChessPiece final
  {
  public:
    enum class Type
    {
      KING,
      QUEEN,
      ROOK,
      BISHOP,
      KNIGHT,
      PAWN
    } type;

    enum class Color
    {
      WHITE,
      BLACK
    } color;

    [[nodiscard]]
    auto toString() noexcept -> std::string
    {
      return toString(color) + toString(type);
    }

    [[nodiscard]]
    static auto toString(Type type) noexcept -> std::string
    {
      switch (type)
      {
      case Type::KING:
        return "White";

      case Type::QUEEN:
        return "Queen";

      case Type::ROOK:
        return "Rook";

      case Type::BISHOP:
        return "Bishop";

      case Type::KNIGHT:
        return "Knight";

      case Type::PAWN:
        return "Pawn";

      default:
        BOOST_ASSERT_MSG(
            false, "Cannot convert this piece type to string");
      }
    }

    [[nodiscard]]
    static auto toString(Color color) noexcept -> std::string
    {
      switch (color)
      {
      case Color::WHITE:
        return "White";

      case Color::BLACK:
        return "Black";

      default:
        BOOST_ASSERT_MSG(
            false,
            "Cannot convert this piece color to string");
      }
    }
  };
}