// Piece.h
#pragma once

#include <array>
#include <string>

namespace bgg
{
  class Piece
  {
  public:
    std::string type;
    std::string color;

    [[nodiscard]] auto toString() const noexcept -> std::string
    {
      return color + type;
    }
  };

  class Color
  {
  public:
    static constexpr const char WHITE[] = "White";
    static constexpr const char BLACK[] = "Black";
    static constexpr const char RED[] = "Red";
  };

  using Position = std::array<char, 3>;

  inline auto operator==(Position const &lhs, Position const &rhs) -> bool
  {
    return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]);
    //&& (lhs[2] == rhs[2]) && (lhs[2] == '\0');
  }
} // namespace bgg
