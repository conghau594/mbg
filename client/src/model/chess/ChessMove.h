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

		class Detail;

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

	class ChessMove::Detail
			: public Variant<Normal, Promotion, EnPassant, Castling>
	{
	public:
		constexpr Detail() noexcept = default;

		template <typename U>
		constexpr Detail(U const &data) noexcept
				: Variant<Normal, Promotion, EnPassant, Castling>(data)
		{
		}

		auto getMoveNumber() const noexcept
				-> int const &;
		auto getColor() const noexcept
				-> Side const &;
		auto getMovedPieceType() const noexcept
				-> EntityType const &;
		auto getSourceSquare() const noexcept
				-> Position const &;
		auto getDestinationSquare() const noexcept
				-> Position const &;
		auto getOpponentKingSquare() const noexcept
				-> Position const &;
		auto getOpponentKingStatus() const noexcept
				-> Side::Status const &;

		void setMoveNumber(int moveNumber) noexcept;
		void setColor(Side const &newColor) noexcept;
		void setMovedPieceType(EntityType const &newType) noexcept;
		void setSourceSquare(Position const &newSquare) noexcept;
		void setDestinationSquare(Position const &newSquare) noexcept;
		void setOpponentKingSquare(Position const &newSquare) noexcept;
		void setOpponentKingStatus(Side::Status const &newStatus) noexcept;

		auto getCapturedPieceType() const noexcept
				-> std::optional<EntityType>;
		auto getPromotedPieceType() const noexcept
				-> std::optional<EntityType>;
		auto getCastlingRookMove() const noexcept
				-> std::optional<std::pair<Position, Position>>;
		auto getEnPassantCaptureSquare() const noexcept
				-> std::optional<Position>;
	};

} // namespace bgg
