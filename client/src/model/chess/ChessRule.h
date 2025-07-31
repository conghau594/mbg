// ChessRule.h
#pragma once

#include <map>

#include "IChessRule.h"

#include "ChessMove.h"
#include "model/PieceSet.h"
#include "ChessPiece.h"

namespace bgg
{

	class ChessRule final : public IChessRule
	{
		ChessPieceFactory pieceFactory_;

		PieceSet whitePieces_;
		PieceSet blackPieces_;
		std::map<int, std::shared_ptr<Piece>> removedPieces_; ///< Maps the move number at which the paired piece is captured,
																													///< or the bitwise negation of the move number (~move)
																													///< if the piece is removed due to promotion.
		std::shared_ptr<Piece> whiteKing_;
		std::shared_ptr<Piece> blackKing_;

		std::list<ChessMove::Detail> moveHistory_;
		int moveCount_;

	public:
		ChessRule(
				std::list<std::tuple<EntityType, Side, Position>> const &initialPlacements) noexcept;

		ChessRule(ChessRule const &other) noexcept;

		auto getMoveCount() const noexcept -> int override;

		auto getLastMove() const noexcept -> ChessMove::Detail const & override;

		auto getPositionStatus(
				Position const &square, Side const &color) const noexcept
				-> Position::Status override;

		auto collectPieces(Side const &color) const noexcept
				-> std::list<std::shared_ptr<Piece>> override;

		// Collects all selectable pieces for the given color
		auto collectSelectablePieces(Side const &color) const noexcept
				-> std::list<std::shared_ptr<Piece>> override;

		// Collects candidate moves for a piece at the given square
		auto collectReachableSquares(
				Position const &square, Side const &color) const noexcept
				-> ReachablePosInfo override;

		// Gets the piece at the specified square
		auto findPiece(Position const &square) const noexcept
				-> std::shared_ptr<Piece> override;

		// Tries to execute a move and returns the action taken
		auto tryMove(ChessMove const &move) const -> ChessMove::Detail override;

		auto commitMove(ChessMove::Detail moveDetail) noexcept -> int override;

		auto revertLastMoveCommit() noexcept -> ChessMove::Detail override;

	private:
		auto getPieceSet(Side const &color) noexcept -> PieceSet &;
		auto getPieceSet(Side const &color) const noexcept -> PieceSet const &;
		auto getKing(Side const &color) const noexcept -> Piece &;

		auto tryParseMove(ChessMove const &move) const -> ChessMove::Detail;

		auto tryParseNormalMove(
				ChessMove const &move,
				EntityType const &movedPieceType,
				std::shared_ptr<Piece> const &capturedPiece,
				ReachablePosInfo const &reachable) const -> ChessMove::Detail;
		auto tryParsePromotionMove(
				ChessMove const &move,
				EntityType const &movedPieceType,
				std::shared_ptr<Piece> const &capturedPiece,
				ReachablePosInfo const &reachable) const -> ChessMove::Detail;
		auto tryParseEnPassantMove(
				ChessMove const &move,
				EntityType const &movedPieceType,
				ReachablePosInfo const &reachable) const -> ChessMove::Detail;
		auto tryParseCastlingMove(
				ChessMove const &move,
				EntityType const &movedPieceType,
				std::shared_ptr<Piece> const &pieceAtDestination,
				ReachablePosInfo const &reachable) const -> ChessMove::Detail;

		auto findAttackers(
				Piece const &piece,
				bool onlyFirst = false) const noexcept
				-> std::list<std::shared_ptr<Piece>>;
		auto isStalemated(Side const &color) const noexcept -> bool;
	};

} // namespace bgg
