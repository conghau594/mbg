// ChessMove.h
#pragma once

#include <optional>
#include <string>

#include "base/Variant.h"
#include "base/EnumBitwises.h"
#include "Piece.h"

namespace bgg
{
  class ChessMove
  {
  public:
    Position fromPos;
    Position toPos;
    std::optional<std::string> promote;

    /////////////////////////////////////////////////////////////////////////////
    enum class Error
    {
      INVALID_FROM_POS = 1 << 0,
      INVALID_TO_POS = 1 << 1,
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

    using Result = Variant<
        Invalid,
        Normal,
        Promotion,
        EnPassant,
        Castling>;
  };
} // namespace bgg
