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

		[[nodiscard]] auto getAllyColor() const noexcept -> std::string const &;

		[[nodiscard]] auto getPiecePlacements() const noexcept -> std::map<Position, Piece> const &;

		[[nodiscard]] auto getSelectablePieces(
				std::string const &color) const noexcept -> std::map<Position, Piece>;

		[[nodiscard]] auto collectCandidateMoves(
				Position const &square) const noexcept -> CandidateChessMoveInfo;

		[[nodiscard]] auto getPiece(
				Position const &square) const noexcept -> std::optional<Piece>;

		// TODO: Need refactor
		[[nodiscard]] auto tryMove(
				ChessMove const &move, 
				std::string const &color) const noexcept -> ChessMove::Action;

		void commitMove(ChessMove::Action const &moveAction) noexcept;

	private:
		[[nodiscard]] auto getEnPassantSquare(
				Position const &square,
				std::string const &color) const noexcept -> std::optional<Position>;

		[[nodiscard]] auto getKingCastlingSquares(
				std::string const &color) const noexcept -> std::list<Position>;

		void updateMoveTracker(
				Piece const &piece,
				Position const &fromSquare,
				Position const &toSquare) noexcept;
	};

} // namespace bgg
