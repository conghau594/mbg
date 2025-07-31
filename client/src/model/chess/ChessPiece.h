// Knight.h
#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "model/Piece.h"

namespace bgg
{
  class IChessRule;

  class NormalChessPiece : public Piece
  {
    IChessRule *const board_;
    const std::vector<std::pair<int, int>> moveVectorList_;
    const int maxMoveVectorFactor_;

  public:
    NormalChessPiece(
        IChessRule *board,
        EntityType const &type,
        Side const &color,
        Position const &square,
        std::vector<std::pair<int, int>> moveVectorList,
        int maxMoveVectorFactor) noexcept;

  protected:
    auto clone() const noexcept -> std::shared_ptr<Piece> override;
    auto canMove() const noexcept -> bool override;
    auto canMoveTo(Position const &square) const noexcept -> bool override;
    auto canCapture(Piece const &piece) const noexcept -> bool override;
    auto collectReachablePositions() const noexcept -> ReachablePosInfo override;
    [[nodiscard]] auto getBoard() const noexcept -> IChessRule *;
  };

  class Knight final : public NormalChessPiece
  {
  public:
    Knight(IChessRule *board,
           Side const &color,
           Position const &square) noexcept;
  };

  class Bishop final : public NormalChessPiece
  {
  public:
    Bishop(IChessRule *board,
           Side const &color,
           Position const &square) noexcept;
  };

  class Rook final : public NormalChessPiece
  {
  public:
    Rook(IChessRule *board,
         Side const &color,
         Position const &square) noexcept;
  };

  class Queen final : public NormalChessPiece
  {
  public:
    Queen(IChessRule *board,
          Side const &color,
          Position const &square) noexcept;
  };

  class King final : public NormalChessPiece
  {
    const Position initialRookSquares_[2];

  public:
    King(IChessRule *board,
         Side const &color,
         Position const &square) noexcept;

  private:
    auto clone() const noexcept -> std::shared_ptr<Piece> override;
    auto canMoveTo(Position const &square) const noexcept -> bool override;
    auto collectReachablePositions() const noexcept
        -> ReachablePosInfo override;
  };

  class Pawn : public Piece
  {
    IChessRule *const board_;
    int const step_;
    int const enPassantRank_;

  public:
    Pawn(IChessRule *board,
         Side const &color,
         Position const &square) noexcept;

  private:
    auto clone() const noexcept -> std::shared_ptr<Piece> override;
    auto canMove() const noexcept -> bool override;
    auto canMoveTo(Position const &square) const noexcept -> bool override;
    auto canCapture(Piece const &pos) const noexcept -> bool override;
    auto collectReachablePositions() const noexcept
        -> ReachablePosInfo override;
    [[nodiscard]] auto findEnPassantDestination() const noexcept
        -> std::optional<Position>;
  };

  class ChessPieceFactory
  {
  public:
    [[nodiscard]] auto createPiece(
        IChessRule *board,
        EntityType type,
        Side color,
        Position square) const noexcept -> std::shared_ptr<Piece>;
  };
} // namespace bgg
