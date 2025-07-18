// ChessMove.h
#pragma once

#include <optional>
#include <string>
#include <list>

#include "base/Variant.h"
#include "base/EnumBitwises.h"
#include "model/Piece.h"

namespace bgg
{
  class CandidateChessMoveInfo final
  {
  public:
    Piece piece;

    std::list<Position> quietSquares;
    std::list<Position> captureSquares;
    std::optional<Position> enPassantSquare;

    void merge(CandidateChessMoveInfo other) noexcept
    {
      quietSquares.splice(quietSquares.end(), other.quietSquares);
      captureSquares.splice(captureSquares.end(), other.captureSquares);

      if (!enPassantSquare && other.enPassantSquare)
      {
        enPassantSquare = std::move(other.enPassantSquare);
      }
    }
  };

  enum class KingStatus : unsigned
  {
    SAFE,
    CHECK,
    CHECKMATE
  };

  /////////////////////////////////////////////////////////////////////////////
  class ChessMove
  {
  public:
    Position fromSquare;
    Position toSquare;
    std::optional<std::string> promote;

    /////////////////////////////////////////////////////////////////////////////
    enum class Error
    {
      INVALID_SOURCE_SQUARE = 1 << 0,
      INVALID_DESTINATION_SQUARE = 1 << 1,
      // INVALID_EN_PASSANT_CAPTURE = 1 << 2,
      INVALID_PROMOTION = 1 << 3,
      INVALID_CASTLING = 1 << 4,
      KING_EXPOSED = 1 << 5,
    };

    class Invalid
    {
    public:
      Error error;
    };

    class Normal
    {
    public:
      Position fromSquare;
      Position toSquare;

      bool canCapture; ///< whether the piece at 'toSquare' is of enemy or not
      KingStatus enemyKingStatus;
    };

    /** \brief just for Pawns
     *
     */
    class Promotion
    {
    public:
      Position fromSquare;
      Position toSquare;

      bool canCapture; ///< whether the piece at 'toSquare' is of enemy or not
      KingStatus enemyKingStatus;
      std::string promote; ///< the piece name, is one of: Queen, Rook, Bishop, Knight
    };

    /** \brief just for Pawns
     *
     */
    class EnPassant
    {
    public:
      Position fromSquare;
      Position toSquare;

      Position enPassantSquare;
      KingStatus enemyKingStatus;
    };

    /** \brief just for Kings
     *
     */
    class Castling
    {
    public:
      Position fromSquare;
      Position toSquare;

      Position rookSource;
      Position rookDestination;
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
