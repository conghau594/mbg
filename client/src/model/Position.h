// Position.h
#pragma once

#include <array>
#include <string>

#include <boost/assert.hpp>
namespace bgg
{
  class Position
  {
    int file_;
    int rank_;

  public:
    enum class Status : unsigned
    {
      OUT_OF_BOARD,
      EMPTY,
      ALLY,
      OPPONENT
    };

    constexpr Position() = default;

    /**
     * \brief Construct a Position object from a string.
     * \param posString This string should be of length 2, e.g., "a1".
     */
    constexpr Position(std::string_view posString) noexcept
        : file_{int(posString[0])}, rank_{int(posString[1])}
    {
    }

    constexpr Position(int file, int rank) noexcept
        : file_(file), rank_(rank)
    {
      // TODO: consider asserting file and rank in range [-128, 127]
    }

    void operator=(std::string_view posString) noexcept
    {
      BOOST_ASSERT_MSG(
          posString.size() == 2,
          "Position must be of length 2, e.g., 'a1'");
      file_ = int(posString[0]);
      rank_ = int(posString[1]);
    }

    void operator=(std::pair<int, int> const &coordinates) noexcept
    {
      file_ = coordinates.first;
      rank_ = coordinates.second;
    }

    // void operator=(Position const &other) noexcept = default;

    [[nodiscard]] auto toString() const noexcept -> std::string
    {
      return std::string{char(file_), char(rank_)};
    }

    [[nodiscard]] constexpr auto getFile() noexcept -> int &
    {
      return file_;
    }

    [[nodiscard]] constexpr auto getRank() noexcept -> int &
    {
      return rank_;
    }

    [[nodiscard]] constexpr auto getFile() const noexcept -> const int &
    {
      return file_;
    }

    [[nodiscard]] constexpr auto getRank() const noexcept -> const int &
    {
      return rank_;
    }

    [[nodiscard]] constexpr auto operator==(
        const Position &other) const noexcept -> bool
    {
      return (file_ == other.file_) && rank_ == other.rank_;
    }

    [[nodiscard]] static auto toString(Status posStatus) noexcept -> std::string
    {
      switch (posStatus)
      {
      case Status::OUT_OF_BOARD:
        return "Out of board";

      case Status::EMPTY:
        return "Empty";

      case Status::ALLY:
        return "Ally";

      case Status::OPPONENT:
        return "Opponent";

      default:
        BOOST_ASSERT_MSG(false, "Invalid 'Position::Status' value");
        return "";
      }
    }
  };
} // namespace bgg
