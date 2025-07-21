// ChessMoveTracker.h
#pragma once

#include "ChessRule.h"

namespace bgg
{

  class SpecialMoveTracker
  {
    // int firstDoubleStepMoveOfPawn_[ChessRule::BOARD_SIDE]{0, 0, 0, 0, 0, 0, 0, 0};
    int firstDoubleStepMoveOfPawn_[ChessRule::BOARD_SIDE]{1, 1, 1, 1, 1, 1, 1, 1};
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

    [[nodiscard]] auto getFirstDoubleStepOfPawn(int col) const noexcept -> int
    {
      BOOST_ASSERT_MSG(
          col >= ChessRule::FIRST_COL && col < ChessRule::FIRST_COL + ChessRule::BOARD_SIDE,
          "Column of pawn must be from 'a' to 'h'");
      return firstDoubleStepMoveOfPawn_[col - ChessRule::FIRST_COL];
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

    void setFirstDoubleStepMoveOfPawn(int col, int moveNumber) noexcept
    {
      BOOST_ASSERT_MSG(
          col >= ChessRule::FIRST_COL && col < ChessRule::FIRST_COL + ChessRule::BOARD_SIDE,
          "Column of pawn must be from 'a' to 'h'");

      BOOST_ASSERT_MSG(
          firstDoubleStepMoveOfPawn_[col - ChessRule::FIRST_COL] == 0,
          "You can only set value to each of 'firstDoubleStepMoveOfPawn_' once");

      firstDoubleStepMoveOfPawn_[int(col) - ChessRule::FIRST_COL] = moveNumber;
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
