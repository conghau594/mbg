// ChessRule.h
#pragma once

#include <list>
#include <array>
#include <map>
#include <optional>

#include <boost/assert.hpp>

#include "Piece.h"
#include "ChessMove.h"

#define BGG_VALIDATE_CHESS_PIECE(type) BOOST_ASSERT_MSG(        \
    (type == KING) || (type == QUEEN) || (type == ROOK) ||      \
        (type == BISHOP) || (type == KNIGHT) || (type == PAWN), \
    "Type of chess piece must be one of 'King', 'Queen', 'Rook', 'Bishop', 'Knight', 'Pawn'")

#define BGG_VALIDATE_COLOR(color) BOOST_ASSERT_MSG(     \
    (color == Color::WHITE) || (color == Color::BLACK), \
    "Player side must be Color::WHITE or Color::BLACK")

#define BGG_VALIDATE_SQUARE(square) BOOST_ASSERT_MSG(                             \
    square[0] >= 'a' && square[0] <= 'h' && square[1] >= '1' && square[1] <= '8', \
    "Position index must be from 'a1' to 'h8'")

// #define BGG_VALIDATE_PIECE(piece) BOOST_ASSERT_MSG(
//     piece >= ChessPiece::WHITE_KING && piece <= ChessPiece::BLACK_PAWN_H,
//     "Piece must be from ChessPiece::WHITE_KING to ChessPiece::BLACK_PAWN_H")

namespace bgg
{
  class ChessRule final
  {
  public:
    static int constexpr PIECE_COUNT = 32;
    static int constexpr BOARD_SIDE = 8;
    // static int constexpr AREA = BOARD_SIDE * BOARD_SIDE;

    static int constexpr FIRST_COL = int('a');
    static int constexpr FIRST_ROW = int('1');

    class PieceType
    {
    public:
      static constexpr const char KING[] = "King";
      static constexpr const char QUEEN[] = "Queen";
      static constexpr const char ROOK[] = "Rook";
      static constexpr const char BISHOP[] = "Bishop";
      static constexpr const char KNIGHT[] = "Knight";
      static constexpr const char PAWN[] = "Pawn";
    };

    static inline std::pair<Position, Piece> INITIAL_PLACEMENTS[PIECE_COUNT] = {
        {Position{"e1"}, Piece{PieceType::KING, Color::WHITE}},
        {Position{"d1"}, Piece{PieceType::QUEEN, Color::WHITE}},
        {Position{"a1"}, Piece{PieceType::ROOK, Color::WHITE}},
        {Position{"h1"}, Piece{PieceType::ROOK, Color::WHITE}},
        {Position{"b1"}, Piece{PieceType::KNIGHT, Color::WHITE}},
        {Position{"g1"}, Piece{PieceType::KNIGHT, Color::WHITE}},
        {Position{"c1"}, Piece{PieceType::BISHOP, Color::WHITE}},
        {Position{"f1"}, Piece{PieceType::BISHOP, Color::WHITE}},
        {Position{"a2"}, Piece{PieceType::PAWN, Color::WHITE}},
        {Position{"b2"}, Piece{PieceType::PAWN, Color::WHITE}},
        {Position{"c2"}, Piece{PieceType::PAWN, Color::WHITE}},
        {Position{"d2"}, Piece{PieceType::PAWN, Color::WHITE}},
        {Position{"e2"}, Piece{PieceType::PAWN, Color::WHITE}},
        {Position{"f2"}, Piece{PieceType::PAWN, Color::WHITE}},
        {Position{"g2"}, Piece{PieceType::PAWN, Color::WHITE}},
        {Position{"h2"}, Piece{PieceType::PAWN, Color::WHITE}},

        {Position{"e8"}, Piece{PieceType::KING, Color::BLACK}},
        {Position{"d8"}, Piece{PieceType::QUEEN, Color::BLACK}},
        {Position{"a8"}, Piece{PieceType::ROOK, Color::BLACK}},
        {Position{"h8"}, Piece{PieceType::ROOK, Color::BLACK}},
        {Position{"b8"}, Piece{PieceType::KNIGHT, Color::BLACK}},
        {Position{"g8"}, Piece{PieceType::KNIGHT, Color::BLACK}},
        {Position{"c8"}, Piece{PieceType::BISHOP, Color::BLACK}},
        {Position{"f8"}, Piece{PieceType::BISHOP, Color::BLACK}},
        {Position{"a7"}, Piece{PieceType::PAWN, Color::BLACK}},
        {Position{"b7"}, Piece{PieceType::PAWN, Color::BLACK}},
        {Position{"c7"}, Piece{PieceType::PAWN, Color::BLACK}},
        {Position{"d7"}, Piece{PieceType::PAWN, Color::BLACK}},
        {Position{"e7"}, Piece{PieceType::PAWN, Color::BLACK}},
        {Position{"f7"}, Piece{PieceType::PAWN, Color::BLACK}},
        {Position{"g7"}, Piece{PieceType::PAWN, Color::BLACK}},
        {Position{"h7"}, Piece{PieceType::PAWN, Color::BLACK}},
    };

    class CandidateMoveInfo final
    {
    public:
      Piece piece;

      std::list<Position> quietMoves;
      std::list<Position> captureMoves;
      std::optional<Position> enPassantCaptureMove; ///< includes: en passant;
    };

  private:
    std::map<Position, Piece> piecePlacements_;
    std::string yourColor_;

  public:
    ChessRule(std::string color) noexcept;
    ChessRule(std::map<Position, Piece> piecePlacements,
              std::string color) noexcept;

    [[nodiscard]]
    auto getYourColor() const noexcept -> std::string const &;
    [[nodiscard]]
    auto getColor(Position const &square) const noexcept -> std::optional<std::string>;
    [[nodiscard]]
    auto getPiecePlacements() noexcept -> std::map<Position, Piece> const &;

    [[nodiscard]]
    auto getSelectablePieces() const noexcept -> std::map<Position, Piece>;
    [[nodiscard]]
    auto getCandidateMoves(Position const &square) const noexcept
        -> std::optional<CandidateMoveInfo>;

    [[nodiscard]]
    auto getPiece(Position const &square) const noexcept -> std::optional<Piece>;

    [[nodiscard]]
    auto tryMove(ChessMove const &move) const noexcept -> ChessMove::Result;

    // TODO: Need refactor
    // void movePiece(
    //     Position const &fromSquare,
    //     Position const &toSquare,
    //     std::optional<std::string> const &promote);

  private:
    auto isSquareMovable(
        Position const &square, std::string const &color) const noexcept -> bool;
    auto isStraightMovePossible(
        Position const &square, std::string const &color) const noexcept -> bool;
    auto isDiagonalMovePossible(
        Position const &square, std::string const &color) const noexcept -> bool;
    auto isKnightMovePossible(
        Position const &square, std::string const &color) const noexcept -> bool;
    auto isPawnMovePossible(
        Position const &square, std::string const &color) const noexcept -> bool;

    auto getStraightMovableSquares(
        Position const &square, unsigned radius) const noexcept -> std::list<Position>;
    auto getDiagonalMovableSquares(
        Position const &square, unsigned radius) const noexcept -> std::list<Position>;
    auto getKnightMovableSquares(
        Position const &square) const noexcept -> std::list<Position>;
    auto getPawnMovableSquares(
        Position const &square) const noexcept -> std::list<Position>;
  };

} // namespace bgg
