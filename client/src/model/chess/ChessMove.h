// ChessMove.h
#pragma once

#include <optional>
#include <string>

#include "base/Variant.h"
#include "base/EnumBitwises.h"
#include "model/Piece.h"

namespace bgg
{
  class CandidateMoveInfo final
  {
  public:
    Piece piece;

    std::list<Position> quietMoves;
    std::list<Position> captureMoves;
    std::optional<Position> enPassantPos; ///< includes: en passant;

    void merge(CandidateMoveInfo other) noexcept
    {
      quietMoves.splice(quietMoves.end(), other.quietMoves);
      captureMoves.splice(captureMoves.end(), other.captureMoves);

      if (!enPassantPos && other.enPassantPos)
      {
        enPassantPos = std::move(other.enPassantPos);
      }
    }
  };

  /////////////////////////////////////////////////////////////////////////////
  class ChessMove
  {
  public:
    Position fromPos;
    Position toPos;
    std::optional<std::string> promote;

    /////////////////////////////////////////////////////////////////////////////
    enum class Error
    {
      INVALID_SOURCE_SQUARE = 1 << 0,
      INVALID_TARGET_SQUARE = 1 << 1,
      INVALID_PROMOTION = 1 << 2,
      INVALID_CASTLING = 1 << 3,
      KING_EXPOSED = 1 << 4,
    };

    enum class KingStatus : unsigned
    {
      SAFE,
      CHECK,
      CHECKMATE
    };

    class Invalid
    {
    public:
      Error error;
    };

    class Normal
    {
    public:
      Position fromPos;
      Position toPos;

      bool canCapture;
      KingStatus enemyKingStatus;
    };

    /** \brief just for Pawns
     *
     */
    class Promotion
    {
    public:
      Position fromPos;
      Position toPos;

      std::string promote;
      bool canCapture;
      KingStatus enemyKingStatus;
    };

    /** \brief just for Pawns
     *
     */
    class EnPassant
    {
    public:
      Position fromPos;
      Position toPos;

      Position capturePosition;
      KingStatus enemyKingStatus;
    };

    /** \brief just for Kings
     *
     */
    class Castling
    {
    public:
      Position fromPos;
      Position toPos;

      Position rookFromPos;
      Position rookToPos;
      KingStatus enemyKingStatus;
    };

    using VariantAction = Variant<
        Invalid,
        Normal,
        Promotion,
        EnPassant,
        Castling>;
  };
} // namespace bgg
