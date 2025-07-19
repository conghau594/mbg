// ChessRule.h
#pragma once

#include <map>
#include <boost/assert.hpp>

#include "model/Piece.h"
#include "model/chess/ChessMove.h"

#define BGG_VALIDATE_CHESS_PIECE(type) BOOST_ASSERT_MSG(            \
    (type == ChessRule::KING) || (type == ChessRule::QUEEN) ||      \
        (type == ChessRule::ROOK) || (type == ChessRule::BISHOP) || \
        (type == ChessRule::KNIGHT) || (type == ChessRule::PAWN),   \
    "Type of chess piece must be one of 'King', 'Queen', 'Rook', 'Bishop', 'Knight', 'Pawn'")

#define BGG_VALIDATE_COLOR(color) BOOST_ASSERT_MSG(     \
    (color == Color::WHITE) || (color == Color::BLACK), \
    "Player side must be Color::WHITE or Color::BLACK")

#define BGG_VALIDATE_SQUARE(square) BOOST_ASSERT_MSG(                             \
    square[0] >= 'a' && square[0] <= 'h' && square[1] >= '1' && square[1] <= '8', \
    "Position index must be from 'a1' to 'h8'")

namespace bgg
{
  class ChessRule
  {
  public:
    static int constexpr PIECE_COUNT = 32;
    static int constexpr BOARD_SIDE = 8;

    static int constexpr FIRST_COL = int('a');
    static int constexpr FIRST_ROW = int('1');

    static constexpr const char KING[] = "King";
    static constexpr const char QUEEN[] = "Queen";
    static constexpr const char ROOK[] = "Rook";
    static constexpr const char BISHOP[] = "Bishop";
    static constexpr const char KNIGHT[] = "Knight";
    static constexpr const char PAWN[] = "Pawn";

    static const std::initializer_list<std::pair<Position, Piece>> INITIAL_PLACEMENTS;

    [[nodiscard]] static auto getPiece(
        std::map<Position, Piece> const &piecePlacements,
        Position const &square) noexcept -> std::optional<Piece>;

    [[nodiscard]] static auto getEnemyColor(
        std::string const &color) noexcept -> std::string;

    static void commitMoveAction(
        std::map<Position, Piece> &piecePlacements,
        ChessMove::VariantAction const &moveAction) noexcept;

    [[nodiscard]] static auto evaluateKingState(
        std::map<Position, Piece> const &piecePlacements,
        std::string const &color,
        bool checkForCheckmate) noexcept -> KingState;

    [[nodiscard]] static auto isPromotionSquare(
        Position const &square,
        std::string const &color) noexcept -> bool;

    [[nodiscard]] static auto getCastlingRookMove(
        Position const &kingTargetSquare,
        std::string const &color) noexcept -> std::pair<Position, Position>;

    [[nodiscard]] static auto getPositionStatus(
        std::map<Position, Piece> const &piecePlacements,
        Position const &square,
        std::string const &color) noexcept -> PositionStatus;

    [[nodiscard]] static auto isOrthogonalMovePossible(
        std::map<Position, Piece> const &piecePlacements,
        Position const &square,
        std::string const &color) noexcept -> bool;

    [[nodiscard]] static auto isDiagonalMovePossible(
        std::map<Position, Piece> const &piecePlacements,
        Position const &square,
        std::string const &color) noexcept -> bool;

    [[nodiscard]] static auto isKnightMovePossible(
        std::map<Position, Piece> const &piecePlacements,
        Position const &square,
        std::string const &color) noexcept -> bool;

    [[nodiscard]] static auto isNormalPawnMovePossible(
        std::map<Position, Piece> const &piecePlacements,
        Position const &square,
        std::string const &color) noexcept -> bool;

    static auto classifyAndCollectSquare(
        std::map<Position, Piece> const &piecePlacements,
        Position const &square,
        std::string const &color,
        std::list<Position> *quietReachableSquares = nullptr,
        std::list<Position> *captureReachableSquares = nullptr) noexcept -> PositionStatus;

    static void collectOrthogonalReachableSquares(
        std::map<Position, Piece> const &piecePlacements,
        Position const &square,
        std::string const &color,
        int radius,
        std::list<Position> *quietReachableSquares = nullptr,
        std::list<Position> *captureReachableSquares = nullptr) noexcept;

    static void collectDiagonalReachableSquares(
        std::map<Position, Piece> const &piecePlacements,
        Position const &square,
        std::string const &color,
        int radius,
        std::list<Position> *quietReachableSquares = nullptr,
        std::list<Position> *captureReachableSquares = nullptr) noexcept;

    static void collectKnightReachableSquares(
        std::map<Position, Piece> const &piecePlacements,
        Position const &square,
        std::string const &color,
        std::list<Position> *quietReachableSquares = nullptr,
        std::list<Position> *captureReachableSquares = nullptr) noexcept;

    static void collectPawnBasicReachableSquares(
        std::map<Position, Piece> const &piecePlacements,
        Position const &square,
        std::string const &color,
        std::list<Position> *quietReachableSquares = nullptr,
        std::list<Position> *captureReachableSquares = nullptr) noexcept;

    static void collectBasicCandidateMoves(
        std::map<Position, Piece> const &piecePlacements,
        Position const &square,
        std::list<Position> *quietReachableSquares,
        std::list<Position> *captureReachableSquares) noexcept;

  private:
    static auto isKingInCheck(
        std::map<Position, Piece> const &piecePlacements,
        std::string const &color) noexcept -> bool;

    static auto getPieceSquares(
        std::map<Position, Piece> const &piecePlacements,
        std::string const &color) noexcept -> std::map<Position, Piece>;
  };
} // namespace bgg
