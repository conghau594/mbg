// ChessPiece.h
#pragma once

#include <string>

#include <boost/assert.hpp>

namespace bgg
{
  class ChessPiece final
  {
    enum class Type
    {
      KING,
      QUEEN,
      ROOK,
      BISHOP,
      KNIGHT,
      PAWN
    } type_;

    enum class Color
    {
      WHITE,
      BLACK
    } color_;

  public:
    [[nodiscard]] auto type() noexcept -> Type & { return type_; }
    [[nodiscard]] auto color() noexcept -> Color & { return color_; }

    [[nodiscard]] auto toString() const noexcept -> std::string
    {
      return toString(color_) + toString(type_);
    }

    [[nodiscard]]
    static auto toString(Type type) noexcept -> std::string
    {
      switch (type)
      {
      case Type::KING:
        return "King";

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
            false, "Cannot convert this chess piece type to string");
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
            false, "Cannot convert this chess piece color to string");
      }
    }

    [[nodiscard]]
    static auto toColor(std::string const &color) noexcept -> Color
    {
      if ("White" == color)
      {
        return Color::WHITE;
      }

      if ("Black" == color)
      {
        return Color::BLACK;
      }

      BOOST_ASSERT_MSG(
          false, "Cannot convert this string to chess piece color");
    }

    [[nodiscard]]
    static auto toType(std::string const &type) noexcept -> Type
    {
      if ("King" == type)
      {
        return Type::KING;
      }

      if ("Queen" == type)
      {
        return Type::QUEEN;
      }

      if ("Rook" == type)
      {
        return Type::ROOK;
      }

      if ("Bishop" == type)
      {
        return Type::BISHOP;
      }

      if ("Knight" == type)
      {
        return Type::KNIGHT;
      }

      if ("Pawn" == type)
      {
        return Type::PAWN;
      }

      BOOST_ASSERT_MSG(
          false, "Cannot convert this string to chess piece type");
    }
  };
} // namespace bgg
