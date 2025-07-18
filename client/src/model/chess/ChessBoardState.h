// ChessBoardState.h
#pragma once

#include <list>
#include <array>
#include <map>
#include <optional>

#include <boost/assert.hpp>

#include "ChessRule.h"
#include "ChessMove.h"
#include "ChessMoveTracker.h"

namespace bgg
{
	class ChessBoardState final
	{
		std::map<Position, Piece> piecePlacements_;
		std::string yourColor_;
		ChessMoveTracker moveTracker_;

	public:
		ChessBoardState(std::string color) noexcept;
		ChessBoardState(std::map<Position, Piece> piecePlacements,
										std::string color) noexcept;

		[[nodiscard]] auto getYourColor() const noexcept
				-> std::string const &;
		[[nodiscard]] auto getColor(Position const &square) const noexcept
				-> std::optional<std::string>;
		[[nodiscard]] auto getPiecePlacements() const noexcept
				-> std::map<Position, Piece> const &;

		[[nodiscard]] auto getSelectablePieces() const noexcept
				-> std::map<Position, Piece>;
		[[nodiscard]] auto getCandidateMoves(Position const &square) const noexcept
				-> CandidateChessMoveInfo;

		[[nodiscard]] auto getPiece(Position const &square) const noexcept
				-> std::optional<Piece>;

		// TODO: Need refactor
		[[nodiscard]] auto tryMove(ChessMove const &move) const noexcept
				-> ChessMove::VariantAction;

		auto commitMoveAction(ChessMove::VariantAction const &moveAction) noexcept -> bool;

	private:
		enum class SquareStatus
		{
			OUT_OF_BOARD,
			EMPTY,
			ALLY,
			ENEMY
		};

		[[nodiscard]] auto getSquareStatus(
				Position const &square,
				std::string const &color) const noexcept -> SquareStatus;
		auto tryAddCandidateMove(
				Position const &square,
				std::string const &color,
				CandidateChessMoveInfo &candidateMoves) const noexcept -> SquareStatus;

		[[nodiscard]] auto isStraightMovePossible(
				Position const &square,
				std::string const &color) const noexcept -> bool;
		[[nodiscard]] auto isDiagonalMovePossible(
				Position const &square,
				std::string const &color) const noexcept -> bool;
		[[nodiscard]] auto isKnightMovePossible(
				Position const &square,
				std::string const &color) const noexcept -> bool;
		[[nodiscard]] auto isPawnMovePossible(
				Position const &square,
				std::string const &color) const noexcept -> bool;
		[[nodiscard]] auto isEnPassantCapturePossible(
				Position const &square,
				std::string const &color) const noexcept -> bool;

		// [[nodiscard]] auto isKingInCheckAt(
		// 		Position const &square,
		// 		std::string const &color) const noexcept -> bool;

		[[nodiscard]] auto getStraightMovableSquares(
				Position const &square,
				std::string const &color,
				int radius) const noexcept -> CandidateChessMoveInfo;
		[[nodiscard]] auto getDiagonalMovableSquares(
				Position const &square,
				std::string const &color,
				int radius) const noexcept -> CandidateChessMoveInfo;
		[[nodiscard]] auto getKnightMovableSquares(
				Position const &square,
				std::string const &color) const noexcept -> CandidateChessMoveInfo;
		[[nodiscard]] auto getPawnMovableSquares(
				Position const &square,
				std::string const &color) const noexcept -> CandidateChessMoveInfo;

		[[nodiscard]] auto getEnPassantSquare(
				Position const &square,
				std::string const &color) const noexcept -> std::optional<Position>;
		[[nodiscard]] auto getValidCastlingSquares(
				Position const &square,
				std::string const &color) const noexcept -> std::list<Position>;
	};

} // namespace bgg
