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
    std::list<Position> specialMoveSquares; ///< includes 'enPassantSquare' and 'castlingSquares'
  };

  /////////////////////////////////////////////////////////////////////////////
  class ChessMove
  {
  public:
    Position fromSquare;
    Position toSquare;
    std::optional<std::string> promote;

    /////////////////////////////////////////////////////////////////////////////
    class Normal
    {
    public:
      Position fromSquare;
      Position toSquare;

      // bool canCapture; ///< whether the piece at 'toSquare' is of enemy or not
      Position enemyKingSquare;
      KingState enemyKingState;
    };

    /** \brief just for Pawns
     *
     */
    class Promotion
    {
    public:
      Position fromSquare;
      Position toSquare;

      std::string promote; ///< the piece name, is one of: Queen, Rook, Bishop, Knight

      Position enemyKingSquare;
      KingState enemyKingState;
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

      Position enemyKingSquare;
      KingState enemyKingState;
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

      Position enemyKingSquare;
      KingState enemyKingState;
    };

    enum class Error
    {
      UNDEFINED = 0,
      INVALID_SOURCE_SQUARE = 1 << 0,
      INVALID_DESTINATION_SQUARE = 1 << 1,
      INVALID_COLOR = 1 << 2,
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

    using Action = Variant<
        Invalid,
        Normal,
        Promotion,
        EnPassant,
        Castling>;
  };

} // namespace bgg

namespace utils
{

  inline auto toString(bgg::ChessMove::Error error) -> std::string
  {
    switch (error)
    {
    case bgg::ChessMove::Error::INVALID_SOURCE_SQUARE:
      return "Invalid source tile. ";

    case bgg::ChessMove::Error::INVALID_DESTINATION_SQUARE:
      return "Invalid destination tile. ";

    case bgg::ChessMove::Error::INVALID_PROMOTION:
      return "Invalid promotion. ";

    case bgg::ChessMove::Error::INVALID_CASTLING:
      return "Invalid castling. ";

    case bgg::ChessMove::Error::KING_EXPOSED:
      return "The king is exposed. ";

    case bgg::ChessMove::Error::UNDEFINED:
    default:
      return "Undefined. ";
    }
  }
} // namespace utils
