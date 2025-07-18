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

		[[nodiscard]] auto getYourColor() const noexcept -> std::string const &;

		[[nodiscard]] auto getPieceColor(
				Position const &square) const noexcept -> std::optional<std::string>;

		[[nodiscard]] auto getPiecePlacements() const noexcept -> std::map<Position, Piece> const &;

		[[nodiscard]] auto getSelectablePieces(
				std::string const &color) const noexcept -> std::map<Position, Piece>;

		[[nodiscard]] auto getCandidateMoves(
				Position const &square) const noexcept -> CandidateChessMoveInfo;

		[[nodiscard]] auto getPiece(
				Position const &square) const noexcept -> std::optional<Piece>;

		// TODO: Need refactor
		[[nodiscard]] auto tryMove(
				ChessMove const &move) const noexcept -> ChessMove::VariantAction;

		auto commitMoveAction(
				ChessMove::VariantAction const &moveAction) noexcept -> bool;

	private:
		[[nodiscard]] auto getEnPassantSquare(
				Position const &square,
				std::string const &color) const noexcept -> std::list<Position>;

		[[nodiscard]] auto getValidCastlingSquares(
				std::string const &color) const noexcept -> std::list<Position>;
	};

} // namespace bgg
