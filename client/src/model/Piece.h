// Piece.h
#pragma once

#include <array>
#include <string>

#include <boost/assert.hpp>

namespace bgg
{
  class Piece
  {
  public:
    std::string type;
    std::string color;

    [[nodiscard]] auto toString() const noexcept -> std::string
    {
      return color + " " + type;
    }
  };

  class Color
  {
  public:
    static constexpr const char WHITE[] = "White";
    static constexpr const char BLACK[] = "Black";
    static constexpr const char RED[] = "Red";
  };

  enum class PositionStatus : unsigned
  {
    OUT_OF_BOARD,
    EMPTY,
    ALLY,
    ENEMY
  };

  enum class KingState : unsigned
  {
    SAFE,
    IN_CHECK,
    CHECKMATED
  };

  using Position = std::array<char, 3>;

} // namespace bgg

namespace utils
{
  inline auto operator==(
      bgg::Position const &lhs, bgg::Position const &rhs) -> bool
  {
    return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]);
    //&& (lhs[2] == rhs[2]) && (lhs[2] == '\0');
  }

  inline auto toString(bgg::PositionStatus posStatus) noexcept -> std::string
  {
    switch (posStatus)
    {
    case bgg::PositionStatus::OUT_OF_BOARD:
      return "Out of board";

    case bgg::PositionStatus::EMPTY:
      return "Empty";

    case bgg::PositionStatus::ALLY:
      return "Ally";

    case bgg::PositionStatus::ENEMY:
      return "Enemy";

    default:
      BOOST_ASSERT_MSG(false, "Invalid 'PositionStatus' value.");
      return "";
    }
  }

  inline auto toString(bgg::KingState kingState) noexcept -> std::string
  {
    switch (kingState)
    {
    case bgg::KingState::SAFE:
      return "Safe";

    case bgg::KingState::IN_CHECK:
      return "In check";

    case bgg::KingState::CHECKMATED:
      return "Checkmated";

    default:
      BOOST_ASSERT_MSG(false, "Invalid 'KingState' value.");
      return "";
    }
  }
} // namespace utils
