// ChessMoveTracker.h
#pragma once

#include "ChessRule.h"

namespace bgg
{

  class SpecialMoveTracker
  {
    int moveInfoOfPawn_[ChessRule::BOARD_SIDE]{0, 0, 0, 0, 0, 0, 0, 0};
    // int moveInfoOfPawn_[ChessRule::BOARD_SIDE]{1, 1, 1, 1, 1, 1, 1, 1};
    bool kingMoved_ = false;
    bool rookAMoved_ = false;
    bool rookHMoved_ = false;

  public:
    [[nodiscard]] auto isKingMoved() const noexcept -> bool
    {
      return kingMoved_;
    }

    [[nodiscard]] auto isRookAMoved() const noexcept -> bool
    {
      return rookAMoved_;
    }
    [[nodiscard]] auto isRookHMoved() const noexcept -> bool
    {
      return rookHMoved_;
    }

    /**
     * \return  0: if the pawn has never moved
     *
     *          -1 or < 0: if the pawn moved but
     *          the first move is not a double step
     *
     *          moveNumber > 0 at which, the pawn moved
     *          with a double step
     */
    [[nodiscard]] auto getMoveInfoOfPawn(int col) const noexcept -> int
    {
      BOOST_ASSERT_MSG(
          col >= ChessRule::FIRST_COL && col < ChessRule::FIRST_COL + ChessRule::BOARD_SIDE,
          "Column of pawn must be from 'a' to 'h'");
      return moveInfoOfPawn_[col - ChessRule::FIRST_COL];
    }

    void markKingMoved() noexcept
    {
      kingMoved_ = true;
    }
    void markRookAMoved() noexcept
    {
      rookAMoved_ = true;
    }
    void markRookHMoved() noexcept
    {
      rookHMoved_ = true;
    }

    /**
     * \param moveNumber  0: specifies that the pawn has never moved
     *
     *                    -1 or < 0: specifies that the pawn moved
     *                    but the first move is not a double step
     *
     *                    > 0: specifies that the pawn moved and
     *                    the first move is a double step at 'moveNumber'
     *                    (a.k.a move count)
     */
    void markPawnMoved(int col, int moveNumber) noexcept
    {
      BOOST_ASSERT_MSG(
          col >= ChessRule::FIRST_COL &&
              col < ChessRule::FIRST_COL + ChessRule::BOARD_SIDE,
          "Column of pawn must be from 'a' to 'h'");

      BOOST_ASSERT_MSG(
          moveInfoOfPawn_[col - ChessRule::FIRST_COL] == 0,
          "You can only set value to each of 'moveInfoOfPawn_' once");

      moveInfoOfPawn_[int(col) - ChessRule::FIRST_COL] = moveNumber;
    }
  };

  /////////////////////////////////////////////////////////////////////////////
  class ChessMoveTracker
  {
    int totalMoveCount_ = 0;
    SpecialMoveTracker white_, black_;

  public:
    [[nodiscard]] auto getSpecialMoveTracker(
        std::string const &color) noexcept -> SpecialMoveTracker &
    {
      BGG_VALIDATE_COLOR(color);
      return (color == Color::WHITE) ? white_ : black_;
    }

    [[nodiscard]] auto getSpecialMoveTracker(
        std::string const &color) const noexcept -> SpecialMoveTracker const &
    {
      BGG_VALIDATE_COLOR(color);
      return (color == Color::WHITE) ? white_ : black_;
    }

    [[nodiscard]] auto getTotalMoveCount() const noexcept -> int
    {
      return totalMoveCount_;
    }
    void increaseTotalMoveCount() noexcept
    {
      ++totalMoveCount_;
    }
  };
} // namespace bgg
