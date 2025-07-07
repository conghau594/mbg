// ChessRule.h
#pragma once

#include <list>
#include <array>
#include <map>
#include <optional>

#include <boost/assert.hpp>

#include "ChessPiece.h"

#define BGG_VALIDATE_SIDE(side) BOOST_ASSERT_MSG(           \
    side == ChessColor::WHITE || side == ChessColor::BLACK, \
    "Player side must be PieceColor::WHITE or PieceColor::BLACK")

#define BGG_VALIDATE_SQUARE(square) BOOST_ASSERT_MSG(                             \
    square[0] >= 'a' && square[0] <= 'h' && square[1] >= '1' && square[1] <= '8', \
    "Square index must be from 'a1' to 'h8'")

#define BGG_VALIDATE_PIECE(piece) BOOST_ASSERT_MSG(                       \
    piece >= ChessPiece::WHITE_KING && piece <= ChessPiece::BLACK_PAWN_H, \
    "Piece must be from ChessPiece::WHITE_KING to ChessPiece::BLACK_PAWN_H")

namespace bgg
{
  class ChessRule final
  {
  public:
    using Square = std::array<char, 3>;
    using Piece = int;

    static std::size_t constexpr PIECE_COUNT = ChessPiece::COUNT;
    static std::pair<Piece, Square> constexpr INITIAL_PLACEMENTS[PIECE_COUNT]{
        {ChessPiece::WHITE_KING, Square{"e1"}},
        {ChessPiece::WHITE_QUEEN, Square{"d1"}},
        {ChessPiece::WHITE_ROOK_A, Square{"a1"}},
        {ChessPiece::WHITE_ROOK_H, Square{"h1"}},
        {ChessPiece::WHITE_KNIGHT_B, Square{"b1"}},
        {ChessPiece::WHITE_KNIGHT_G, Square{"g1"}},
        {ChessPiece::WHITE_BISHOP_C, Square{"c1"}},
        {ChessPiece::WHITE_BISHOP_F, Square{"f1"}},
        {ChessPiece::WHITE_PAWN_A, Square{"a2"}},
        {ChessPiece::WHITE_PAWN_B, Square{"b2"}},
        {ChessPiece::WHITE_PAWN_C, Square{"c2"}},
        {ChessPiece::WHITE_PAWN_D, Square{"d2"}},
        {ChessPiece::WHITE_PAWN_E, Square{"e2"}},
        {ChessPiece::WHITE_PAWN_F, Square{"f2"}},
        {ChessPiece::WHITE_PAWN_G, Square{"g2"}},
        {ChessPiece::WHITE_PAWN_H, Square{"h2"}},

        {ChessPiece::BLACK_KING, Square{"e8"}},
        {ChessPiece::BLACK_QUEEN, Square{"d8"}},
        {ChessPiece::BLACK_ROOK_A, Square{"a8"}},
        {ChessPiece::BLACK_ROOK_H, Square{"h8"}},
        {ChessPiece::BLACK_KNIGHT_B, Square{"b8"}},
        {ChessPiece::BLACK_KNIGHT_G, Square{"g8"}},
        {ChessPiece::BLACK_BISHOP_C, Square{"c8"}},
        {ChessPiece::BLACK_BISHOP_F, Square{"f8"}},
        {ChessPiece::BLACK_PAWN_A, Square{"a7"}},
        {ChessPiece::BLACK_PAWN_B, Square{"b7"}},
        {ChessPiece::BLACK_PAWN_C, Square{"c7"}},
        {ChessPiece::BLACK_PAWN_D, Square{"d7"}},
        {ChessPiece::BLACK_PAWN_E, Square{"e7"}},
        {ChessPiece::BLACK_PAWN_F, Square{"f7"}},
        {ChessPiece::BLACK_PAWN_G, Square{"g7"}},
        {ChessPiece::BLACK_PAWN_H, Square{"h7"}}};

    class PiecePlacement
    {
    public:
      Piece piece;
      Square square;
    };

    class CandidateMoveInfo
    {
    public:
      PiecePlacement piecePlacement;

      std::list<Square> quietMoves;
      std::list<Square> captureMoves;
      std::optional<Square> specialMove; ///< includes: castling, promotion, en passant;
    };

    class Board
    {
    public:
      static int constexpr SIDE_LENGTH = 8;
      static int constexpr AREA = SIDE_LENGTH * SIDE_LENGTH;
      static int constexpr EMPTY_SQUARE = -1;
      static int constexpr FIRST_COL = int('a');
      static int constexpr FIRST_ROW = int('1');

    private:
      Piece pieceMap_[AREA];

    public:
      Board() noexcept;
      auto operator[](Square const &square) noexcept -> int &;
    };

  private:
    std::map<Piece, Square> piecePlacements_;
    Board board_;
    int side_;

  public:
    ChessRule(int side) noexcept;
    auto getPiecePlacements() const noexcept -> std::map<Piece, Square> const &;

    auto getSelectablePieces() const noexcept -> std::list<PiecePlacement>;
    auto getCandidateMoves(Square const &square) const noexcept
        -> std::optional<CandidateMoveInfo>;
    auto getPiece(Square const &square) const noexcept -> int;
    auto getSquare(int piece) const noexcept -> Square;

  private:
  };

} // namespace bgg
