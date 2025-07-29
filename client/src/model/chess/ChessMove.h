// ChessMove.h
#pragma once

#include <optional>
#include <string>

#include "base/Variant.h"
#include "base/EnumBitwises.h"
#include "model/EntityType.h"
#include "model/Side.h"
#include "model/Position.h"

namespace bgg
{
  /////////////////////////////////////////////////////////////////////////////
  class ChessMove
  {
  public:
    Side color;
    Position fromSquare;
    Position toSquare;
    std::optional<EntityType> promotedPiece;

    /////////////////////////////////////////////////////////////////////////////
    class Normal
    {
    public:
      Side color;

      Position fromSquare;
      Position toSquare;
      EntityType movedPiece;

      std::optional<EntityType> capturedPiece;
      Position opponentKingSquare;
      Side::Status opponentKingStatus;
    };

    /** \brief just for Pawns
     *
     */
    class Promotion
    {
    public:
      Side color;

      Position fromSquare;
      Position toSquare;
      EntityType movedPiece;

      EntityType promotedPiece; ///< the piece name, is one of: Queen, Rook, Bishop, Knight

      std::optional<EntityType> capturedPiece;
      Position opponentKingSquare;
      Side::Status opponentKingStatus;
    };

    /** \brief just for Pawns
     *
     */
    class EnPassant
    {
    public:
      Side color;

      Position fromSquare;
      Position toSquare;
      EntityType movedPiece;

      Position enPassantCaptureSquare;
      EntityType capturedPiece;

      Position opponentKingSquare;
      Side::Status opponentKingStatus;
    };

    /** \brief just for Kings
     *
     */
    class Castling
    {
    public:
      Side color;

      Position fromSquare;
      Position toSquare;
      EntityType movedPiece;

      Position rookSource;
      Position rookDestination;

      Position opponentKingSquare;
      Side::Status opponentKingStatus;
    };

    class Invalid
    {
    public:
      Side color;

      Position fromSquare;
      Position toSquare;
      std::optional<EntityType> promotedPiece;

      std::string errorMessage;
    };

    using Detail = Variant<
        Invalid,
        Normal,
        Promotion,
        EnPassant,
        Castling>;

    static auto getColor(Detail const &moveDetail) noexcept
        -> Side const &;
    static auto getMovedPieceType(Detail const &moveDetail) noexcept
        -> EntityType const &;
    static auto getSourceSquare(Detail const &moveDetail) noexcept
        -> Position const &;
    static auto getDestinationSquare(Detail const &moveDetail) noexcept
        -> Position const &;
    static auto getOpponentKingSquare(Detail const &moveDetail) noexcept
        -> Position const &;
    static auto getOpponentKingStatus(Detail const &moveDetail) noexcept
        -> Side::Status const &;
    static auto getCapturedPieceType(Detail const &moveDetail) noexcept
        -> std::optional<EntityType>;
    static auto getPromotedPieceType(Detail const &moveDetail) noexcept
        -> std::optional<EntityType>;
    static auto getRookSourceSquare(Detail const &moveDetail) noexcept
        -> std::optional<Position>;
    static auto getRookDestinationSquare(Detail const &moveDetail) noexcept
        -> std::optional<Position>;
    static auto getEnPassantCaptureSquare(Detail const &moveDetail) noexcept
        -> std::optional<Position>;
    static auto getErrorMessage(Detail const &moveDetail) noexcept
        -> std::string;
  };

} // namespace bgg
