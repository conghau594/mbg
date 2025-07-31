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
            int moveNumber;
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
            int moveNumber;
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
            int moveNumber;
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
            int moveNumber;
            Side color;

            Position fromSquare;
            Position toSquare;
            EntityType movedPiece;

            Position rookSource;
            Position rookDestination;

            Position opponentKingSquare;
            Side::Status opponentKingStatus;
        };

        using Detail = Variant<
            // Invalid,
            Normal,
            Promotion,
            EnPassant,
            Castling>;

        static auto getMoveNumber(Detail const &moveDetail) noexcept
            -> int const &;
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

        static void setMoveNumber(
            Detail &moveDetail, int moveNumber) noexcept;
        static void setColor(
            Detail &moveDetail, Side const &newColor) noexcept;
        static void setMovedPieceType(
            Detail &moveDetail, EntityType const &newType) noexcept;
        static void setSourceSquare(
            Detail &moveDetail, Position const &newSquare) noexcept;
        static void setDestinationSquare(
            Detail &moveDetail, Position const &newSquare) noexcept;
        static void setOpponentKingSquare(
            Detail &moveDetail, Position const &newSquare) noexcept;
        static void setOpponentKingStatus(
            Detail &moveDetail, Side::Status const &newStatus) noexcept;

        static auto getCapturedPieceType(Detail const &moveDetail) noexcept
            -> std::optional<EntityType>;
        static auto getPromotedPieceType(Detail const &moveDetail) noexcept
            -> std::optional<EntityType>;
        static auto getCastlingRookMove(Detail const &moveDetail) noexcept
            -> std::optional<std::pair<Position, Position>>;
        static auto getEnPassantCaptureSquare(Detail const &moveDetail) noexcept
            -> std::optional<Position>;

        static auto generateMinimalNormalMove(
            ChessMove const &move,
            bool shouldCapture) noexcept -> ChessMove::Normal;
        static auto generateMinimalPromotionMove(
            ChessMove const &move,
            bool shouldCapture) noexcept -> ChessMove::Promotion;
        static auto generateMinimalEnPassantMove(
            ChessMove const &move,
            Position const &enPassantCaptureSquare) noexcept -> ChessMove::EnPassant;
        static auto generateMinimalCastlingMove(
            ChessMove const &move,
            Position const &rookSource,
            Position const &rookDestination) noexcept -> ChessMove::Castling;
    };

} // namespace bgg
