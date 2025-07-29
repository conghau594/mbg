// IChessRule.h
#pragma once

#include <memory>
#include <list>

#include "model/Position.h"
#include "ChessMove.h"

namespace bgg
{
    class Side;
    class Piece;
    class ReachablePosInfo;

    class IChessRule
    {
    public:
        virtual ~IChessRule() = default;

        [[nodiscard]] virtual auto getMoveCount() const -> int = 0;

        [[nodiscard]] virtual auto getLastMove() const
            -> ChessMove::Detail const & = 0;

        [[nodiscard]] virtual auto getPositionStatus(
            Position const &square, Side const &color) const
            -> Position::Status = 0;

        [[nodiscard]] virtual auto collectPieces(Side const &color) const
            -> std::list<std::shared_ptr<Piece>> = 0;

        // Collects all selectable pieces for the given color
        [[nodiscard]] virtual auto collectSelectablePieces(Side const &color) const
            -> std::list<std::shared_ptr<Piece>> = 0;

        // Collects candidate moves for a piece at the given square
        [[nodiscard]] virtual auto collectReachableSquares(
            Position const &square, Side const &color) const
            -> ReachablePosInfo = 0;

        // Gets the piece at the specified square
        [[nodiscard]] virtual auto findPiece(Position const &square)
            -> std::shared_ptr<Piece> = 0;

        // Tries to execute a move and returns the action taken
        [[nodiscard]] virtual auto tryMove(ChessMove const &move) const
            -> ChessMove::Detail = 0;

        virtual void commitMove(ChessMove::Detail moveAction) = 0;
    };

} // namespace bgg
