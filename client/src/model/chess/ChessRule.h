// ChessRule.h
#pragma once

#include "IChessRule.h"

#include "ChessMove.h"
#include "model/PieceSet.h"

namespace bgg
{

  class ChessRule final : public IChessRule
  {
    PieceSet whitePieces_;
    PieceSet blackPieces_;

    std::list<ChessMove::Detail> moveHistory_;
    int moveCount_;

  public:
    ChessRule(
        std::list<std::tuple<EntityType, Side, Position>> const &initialPlacements) noexcept;

    auto getMoveCount() const noexcept -> int override;

    auto getLastMove() const noexcept -> ChessMove::Detail const & override;

    auto getPositionStatus(
        Position const &square, Side const &color) const noexcept
        -> Position::Status override;

    auto collectPieces(Side const &color) const noexcept
        -> std::list<std::shared_ptr<Piece>> override;

    // Collects all selectable pieces for the given color
    auto collectSelectablePieces(Side const &color) const noexcept
        -> std::list<std::shared_ptr<Piece>> override;

    // Collects candidate moves for a piece at the given square
    auto collectReachableSquares(
        Position const &square, Side const &color) const noexcept
        -> ReachablePosInfo override;

    // Gets the piece at the specified square
    auto findPiece(Position const &square) noexcept
        -> std::shared_ptr<Piece> override;

    // Tries to execute a move and returns the action taken
    auto tryMove(ChessMove const &move) const noexcept
        -> ChessMove::Detail override;

    void commitMove(ChessMove::Detail moveAction) noexcept override;

  private:
    auto getPieceSet(Side const &color) noexcept -> PieceSet *;
    auto getPieceSet(Side const &color) const noexcept -> PieceSet const *;
  };

} // namespace bgg
