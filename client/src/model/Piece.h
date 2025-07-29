// Piece.h
#pragma once

#include <list>

#include "EntityType.h"
#include "Side.h"
#include "Position.h"

namespace bgg
{
  class ReachablePosInfo final
  {
  public:
    std::list<Position> quietPositions;
    std::list<Position> capturePositions;
    std::list<Position> specialPositions;
  };

  class Piece
  {
    EntityType const type_;
    Side const side_;
    Position position_;
    int moveCount_;

  public:
    constexpr Piece(EntityType const &type,
                    Side const &side,
                    Position const &pos) noexcept
        : type_(type),
          side_(side),
          position_(pos),
          moveCount_(0)
    {
    }

    virtual ~Piece() = default;

    [[nodiscard]] constexpr auto getPosition() const noexcept -> Position const &
    {
      return position_;
    }

    [[nodiscard]] auto isMoved() const noexcept -> bool
    {
      return moveCount_ != 0;
    }

    [[nodiscard]] auto getMoveCount() const noexcept -> int const &
    {
      return moveCount_;
    }

    [[nodiscard]] constexpr auto getSide() const noexcept -> Side const &
    {
      return side_;
    }

    [[nodiscard]] constexpr auto getType() const noexcept -> EntityType const &
    {
      return type_;
    }

    virtual void move(Position const &newPosition) noexcept
    {
      ++moveCount_;
      position_ = newPosition;
    }

    [[nodiscard]] virtual auto canMove() const -> bool = 0;

    [[nodiscard]] virtual auto collectReachablePositions() const -> ReachablePosInfo = 0;
  };
} // namespace bgg
