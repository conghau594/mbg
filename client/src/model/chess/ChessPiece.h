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
    const std::vector<std::pair<int, int>> moveVectorList_;
    const int maxMoveVectorFactor_;

  public:
    NormalChessPiece(
        IChessRule const *rule,
        EntityType const &type,
        Side const &color,
        Position const &square,
        std::vector<std::pair<int, int>> moveVectorList,
        int maxMoveVectorFactor) noexcept;

    NormalChessPiece(NormalChessPiece const &other,
                     IChessRule const *rule) noexcept;

  protected:
    auto clone(IChessRule const *rule) const noexcept
        -> std::shared_ptr<Piece> override;
    auto canMove() const noexcept -> bool override;
    auto canMoveTo(Position const &square) const noexcept -> bool override;
    auto canCapture(Piece const &piece) const noexcept -> bool override;
    auto collectReachablePositions() const noexcept -> ReachablePosInfo override;
  };

  class Knight final : public NormalChessPiece
  {
  public:
    Knight(IChessRule const *rule,
           Side const &color,
           Position const &square) noexcept;
  };

  class Bishop final : public NormalChessPiece
  {
  public:
    Bishop(IChessRule const *rule,
           Side const &color,
           Position const &square) noexcept;
  };

  class Rook final : public NormalChessPiece
  {
  public:
    Rook(IChessRule const *rule,
         Side const &color,
         Position const &square) noexcept;
  };

  class Queen final : public NormalChessPiece
  {
  public:
    Queen(IChessRule const *rule,
          Side const &color,
          Position const &square) noexcept;
  };

  class King final : public NormalChessPiece
  {
    const Position initialRookSquares_[2];

  public:
    King(IChessRule const *rule,
         Side const &color,
         Position const &square) noexcept;
    King(King const &other, IChessRule const *rule) noexcept;

  private:
    auto clone(IChessRule const *rule) const noexcept
        -> std::shared_ptr<Piece> override;
    auto canMoveTo(Position const &square) const noexcept
        -> bool override;
    auto collectReachablePositions() const noexcept
        -> ReachablePosInfo override;

    /**
     * \return the destination of rook if castling with the rook is possible
     */
    auto findRookCastlingDestination(Position const &rookSource) const noexcept
        -> std::optional<Position>;
  };

  class Pawn : public Piece
  {
    int const step_;
    int const enPassantRank_;

  public:
    Pawn(IChessRule const *rule,
         Side const &color,
         Position const &square) noexcept;
    Pawn(Pawn const &other, IChessRule const *rule) noexcept;

  private:
    auto clone(IChessRule const *rule) const noexcept -> std::shared_ptr<Piece> override;
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
        IChessRule *rule,
        EntityType type,
        Side color,
        Position square) const noexcept -> std::shared_ptr<Piece>;
  };
} // namespace bgg
