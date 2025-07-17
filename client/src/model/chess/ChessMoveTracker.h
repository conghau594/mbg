// ChessMoveTracker.h
#pragma once

#include "ChessUtils.h"

namespace bgg
{

  class SpecialPieceMoveTracker
  {
    int firstDoubleStepMoveOfPawn_[ChessUtils::BOARD_SIDE]{0, 0, 0, 0, 0, 0, 0, 0};
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

    [[nodiscard]] auto getFirstDoubleStepMoveOfPawn(int col) const noexcept -> int
    {
      BOOST_ASSERT_MSG(
          col >= ChessUtils::FIRST_COL && col < ChessUtils::FIRST_COL + ChessUtils::BOARD_SIDE,
          "Column of pawn must be from 'a' to 'h'");
      return firstDoubleStepMoveOfPawn_[col - ChessUtils::FIRST_COL];
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
          col >= ChessUtils::FIRST_COL && col < ChessUtils::FIRST_COL + ChessUtils::BOARD_SIDE,
          "Column of pawn must be from 'a' to 'h'");

      BOOST_ASSERT_MSG(
          firstDoubleStepMoveOfPawn_[col - ChessUtils::FIRST_COL] == 0,
          "You are allowed to set each of 'firstDoubleStepMoveOfPawn_' only once");

      firstDoubleStepMoveOfPawn_[int(col) - ChessUtils::FIRST_COL] = moveNumber;
    }
  };

  /////////////////////////////////////////////////////////////////////////////
  class ChessMoveTracker
  {
    int totalMoveCount_ = 0;
    SpecialPieceMoveTracker white_, black_;

  public:
    [[nodiscard]] auto getSpecialPieceMoveTracker(
        std::string const &color) noexcept -> SpecialPieceMoveTracker &
    {
      BGG_VALIDATE_COLOR(color);
      return (color == Color::WHITE) ? white_ : black_;
    }

    [[nodiscard]] auto getSpecialPieceMoveTracker(
        std::string const &color) const noexcept -> SpecialPieceMoveTracker const &
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
