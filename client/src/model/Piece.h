// Piece.h
#pragma once

#include <list>

#include "EntityType.h"
#include "Side.h"
#include "Position.h"

#include "base/Logger.h"

namespace bgg
{
  class IChessRule;
  class ReachablePosInfo final
  {
  public:
    std::list<Position> quietPositions;
    std::list<Position> capturePositions;
    std::list<Position> specialPositions;
  };

  class Piece
  {
    IChessRule const *rule_;
    EntityType const type_;
    Side const side_;
    Position position_;
    int moveCount_;

  public:
    constexpr Piece(IChessRule const *rule,
                    EntityType const &type,
                    Side const &side,
                    Position const &pos) noexcept
        : rule_(rule),
          type_(type),
          side_(side),
          position_(pos),
          moveCount_(0)
    {
    }

    virtual ~Piece()
    {
      // SPDLOG_INFO("A {} {} at '{}' has destroyed",
      //             side_.toString(),
      //             type_.toString(),
      //             position_.toString());
    }

    [[nodiscard]] constexpr auto getPosition() const noexcept -> Position const &
    {
      return position_;
    }

    /**
     * \brief Silently sets the position of the piece to `newPos`,
     *        without inscreasing the move count
     */
    [[nodiscard]] constexpr void setMoveInfo(
        Position const &newPos, int newMoveCount) noexcept
    {
      BOOST_ASSERT_MSG(
          newMoveCount >= 0, "Cannot set moveCount_ to a negative number");
      position_ = newPos;
      moveCount_ = newMoveCount;
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

    [[nodiscard]] virtual auto clone(IChessRule const *rule) const
        -> std::shared_ptr<Piece> = 0;
    [[nodiscard]] virtual auto canMove() const -> bool = 0;
    [[nodiscard]] virtual auto canMoveTo(Position const &pos) const -> bool = 0;
    [[nodiscard]] virtual auto canCapture(Piece const &piece) const -> bool = 0;
    [[nodiscard]] virtual auto collectReachablePositions() const
        -> ReachablePosInfo = 0;

  protected:
    [[nodiscard]] auto getRule() const noexcept -> IChessRule const *
    {
      return rule_;
    }
  };
} // namespace bgg
