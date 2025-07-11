// ChessRule.h
#pragma once

#include <list>
#include <array>
#include <map>
#include <optional>

#include <boost/assert.hpp>

#include "ChessPiece.h"

#define BGG_VALIDATE_COLOR(color) BOOST_ASSERT_MSG(                             \
    (color == ChessPiece::Color::WHITE) || (color == ChessPiece::Color::BLACK), \
    "Player side must be ChessPiece::Color::WHITE or ChessPiece::Color::BLACK")

#define BGG_VALIDATE_SQUARE(square) BOOST_ASSERT_MSG(                             \
    square[0] >= 'a' && square[0] <= 'h' && square[1] >= '1' && square[1] <= '8', \
    "Square index must be from 'a1' to 'h8'")

// #define BGG_VALIDATE_PIECE(piece) BOOST_ASSERT_MSG(
//     piece >= ChessPiece::WHITE_KING && piece <= ChessPiece::BLACK_PAWN_H,
//     "Piece must be from ChessPiece::WHITE_KING to ChessPiece::BLACK_PAWN_H")

namespace bgg
{
  class ChessRule final
  {
  public:
    using Square = std::array<char, 3>;
    static int constexpr PIECE_COUNT = 32;
    static int constexpr BOARD_SIDE = 8;
    // static int constexpr AREA = BOARD_SIDE * BOARD_SIDE;

    static int constexpr FIRST_COL = int('a');
    static int constexpr FIRST_ROW = int('1');

    static std::pair<Square, ChessPiece> constexpr INITIAL_PLACEMENTS[PIECE_COUNT]{
        {Square{"e1"}, {ChessPiece::Type::KING, ChessPiece::Color::WHITE}},
        {Square{"d1"}, {ChessPiece::Type::QUEEN, ChessPiece::Color::WHITE}},
        {Square{"a1"}, {ChessPiece::Type::ROOK, ChessPiece::Color::WHITE}},
        {Square{"h1"}, {ChessPiece::Type::ROOK, ChessPiece::Color::WHITE}},
        {Square{"b1"}, {ChessPiece::Type::KNIGHT, ChessPiece::Color::WHITE}},
        {Square{"g1"}, {ChessPiece::Type::KNIGHT, ChessPiece::Color::WHITE}},
        {Square{"c1"}, {ChessPiece::Type::BISHOP, ChessPiece::Color::WHITE}},
        {Square{"f1"}, {ChessPiece::Type::BISHOP, ChessPiece::Color::WHITE}},
        {Square{"a2"}, {ChessPiece::Type::PAWN, ChessPiece::Color::WHITE}},
        {Square{"b2"}, {ChessPiece::Type::PAWN, ChessPiece::Color::WHITE}},
        {Square{"c2"}, {ChessPiece::Type::PAWN, ChessPiece::Color::WHITE}},
        {Square{"d2"}, {ChessPiece::Type::PAWN, ChessPiece::Color::WHITE}},
        {Square{"e2"}, {ChessPiece::Type::PAWN, ChessPiece::Color::WHITE}},
        {Square{"f2"}, {ChessPiece::Type::PAWN, ChessPiece::Color::WHITE}},
        {Square{"g2"}, {ChessPiece::Type::PAWN, ChessPiece::Color::WHITE}},
        {Square{"h2"}, {ChessPiece::Type::PAWN, ChessPiece::Color::WHITE}},

        {Square{"e8"}, {ChessPiece::Type::KING, ChessPiece::Color::BLACK}},
        {Square{"d8"}, {ChessPiece::Type::QUEEN, ChessPiece::Color::BLACK}},
        {Square{"a8"}, {ChessPiece::Type::ROOK, ChessPiece::Color::BLACK}},
        {Square{"h8"}, {ChessPiece::Type::ROOK, ChessPiece::Color::BLACK}},
        {Square{"b8"}, {ChessPiece::Type::KNIGHT, ChessPiece::Color::BLACK}},
        {Square{"g8"}, {ChessPiece::Type::KNIGHT, ChessPiece::Color::BLACK}},
        {Square{"c8"}, {ChessPiece::Type::BISHOP, ChessPiece::Color::BLACK}},
        {Square{"f8"}, {ChessPiece::Type::BISHOP, ChessPiece::Color::BLACK}},
        {Square{"a7"}, {ChessPiece::Type::PAWN, ChessPiece::Color::BLACK}},
        {Square{"b7"}, {ChessPiece::Type::PAWN, ChessPiece::Color::BLACK}},
        {Square{"c7"}, {ChessPiece::Type::PAWN, ChessPiece::Color::BLACK}},
        {Square{"d7"}, {ChessPiece::Type::PAWN, ChessPiece::Color::BLACK}},
        {Square{"e7"}, {ChessPiece::Type::PAWN, ChessPiece::Color::BLACK}},
        {Square{"f7"}, {ChessPiece::Type::PAWN, ChessPiece::Color::BLACK}},
        {Square{"g7"}, {ChessPiece::Type::PAWN, ChessPiece::Color::BLACK}},
        {Square{"h7"}, {ChessPiece::Type::PAWN, ChessPiece::Color::BLACK}},
    };

    class CandidateMoveInfo final
    {
    public:
      ChessPiece piece;

      std::list<Square> quietMoves;
      std::list<Square> captureMoves;
      std::optional<Square> specialMove; ///< includes: castling, promotion, en passant;
    };

  private:
    std::map<Square, ChessPiece> piecePlacements_;
    ChessPiece::Color color_;

  public:
    ChessRule(ChessPiece::Color color) noexcept;
    ChessRule(std::map<Square, ChessPiece> piecePlacements,
              ChessPiece::Color color) noexcept;

    [[nodiscard]]
    auto getColor() const noexcept -> ChessPiece::Color;
    [[nodiscard]]
    auto getPiecePlacements() noexcept -> std::map<Square, ChessPiece> &;

    [[nodiscard]]
    auto getSelectablePieces() const noexcept -> std::map<Square, ChessPiece>;
    [[nodiscard]]
    auto getCandidateMoves(Square const &square) const noexcept
        -> std::optional<CandidateMoveInfo>;

    [[nodiscard]]
    auto getPiece(Square const &square) const noexcept -> std::optional<ChessPiece>;

  private:
    // static constexpr auto indexToSquare(int index) -> Square;
    // static constexpr auto squareToIndex(Square const &square) -> std::size_t;
  };

} // namespace bgg
