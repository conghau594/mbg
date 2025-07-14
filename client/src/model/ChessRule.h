// ChessRule.h
#pragma once

#include <list>
#include <array>
#include <map>
#include <optional>

#include <boost/assert.hpp>

#include "Piece.h"

#define BGG_VALIDATE_CHESS_PIECE(type) BOOST_ASSERT_MSG(        \
    (type == KING) || (type == QUEEN) || (type == ROOK) ||      \
        (type == BISHOP) || (type == KNIGHT) || (type == PAWN), \
    "Type of chess piece must be one of 'King', 'Queen', 'Rook', 'Bishop', 'Knight', 'Pawn'")

#define BGG_VALIDATE_COLOR(color) BOOST_ASSERT_MSG(     \
    (color == Color::WHITE) || (color == Color::BLACK), \
    "Player side must be Color::WHITE or Color::BLACK")

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
    using Square = Position;
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

    static inline std::pair<Square, Piece> INITIAL_PLACEMENTS[PIECE_COUNT] = {
        {Square{"e1"}, Piece{PieceType::KING, Color::WHITE}},
        {Square{"d1"}, Piece{PieceType::QUEEN, Color::WHITE}},
        {Square{"a1"}, Piece{PieceType::ROOK, Color::WHITE}},
        {Square{"h1"}, Piece{PieceType::ROOK, Color::WHITE}},
        {Square{"b1"}, Piece{PieceType::KNIGHT, Color::WHITE}},
        {Square{"g1"}, Piece{PieceType::KNIGHT, Color::WHITE}},
        {Square{"c1"}, Piece{PieceType::BISHOP, Color::WHITE}},
        {Square{"f1"}, Piece{PieceType::BISHOP, Color::WHITE}},
        {Square{"a2"}, Piece{PieceType::PAWN, Color::WHITE}},
        {Square{"b2"}, Piece{PieceType::PAWN, Color::WHITE}},
        {Square{"c2"}, Piece{PieceType::PAWN, Color::WHITE}},
        {Square{"d2"}, Piece{PieceType::PAWN, Color::WHITE}},
        {Square{"e2"}, Piece{PieceType::PAWN, Color::WHITE}},
        {Square{"f2"}, Piece{PieceType::PAWN, Color::WHITE}},
        {Square{"g2"}, Piece{PieceType::PAWN, Color::WHITE}},
        {Square{"h2"}, Piece{PieceType::PAWN, Color::WHITE}},

        {Square{"e8"}, Piece{PieceType::KING, Color::BLACK}},
        {Square{"d8"}, Piece{PieceType::QUEEN, Color::BLACK}},
        {Square{"a8"}, Piece{PieceType::ROOK, Color::BLACK}},
        {Square{"h8"}, Piece{PieceType::ROOK, Color::BLACK}},
        {Square{"b8"}, Piece{PieceType::KNIGHT, Color::BLACK}},
        {Square{"g8"}, Piece{PieceType::KNIGHT, Color::BLACK}},
        {Square{"c8"}, Piece{PieceType::BISHOP, Color::BLACK}},
        {Square{"f8"}, Piece{PieceType::BISHOP, Color::BLACK}},
        {Square{"a7"}, Piece{PieceType::PAWN, Color::BLACK}},
        {Square{"b7"}, Piece{PieceType::PAWN, Color::BLACK}},
        {Square{"c7"}, Piece{PieceType::PAWN, Color::BLACK}},
        {Square{"d7"}, Piece{PieceType::PAWN, Color::BLACK}},
        {Square{"e7"}, Piece{PieceType::PAWN, Color::BLACK}},
        {Square{"f7"}, Piece{PieceType::PAWN, Color::BLACK}},
        {Square{"g7"}, Piece{PieceType::PAWN, Color::BLACK}},
        {Square{"h7"}, Piece{PieceType::PAWN, Color::BLACK}},
    };

    class CandidateMoveInfo final
    {
    public:
      Piece piece;

      std::list<Square> quietMoves;
      std::list<Square> captureMoves;
      std::optional<Square> specialMove; ///< includes: castling, promotion, en passant;
    };

  private:
    std::map<Square, Piece> piecePlacements_;
    std::string yourColor_;

  public:
    ChessRule(std::string color) noexcept;
    ChessRule(std::map<Square, Piece> piecePlacements,
              std::string color) noexcept;

    [[nodiscard]]
    auto getYourColor() const noexcept -> std::string const &;
    [[nodiscard]]
    auto getColor(Square const &square) const noexcept -> std::optional<std::string>;
    [[nodiscard]]
    auto getPiecePlacements() noexcept -> std::map<Square, Piece> const &;

    [[nodiscard]]
    auto getSelectablePieces() const noexcept -> std::map<Square, Piece>;
    [[nodiscard]]
    auto getCandidateMoves(Square const &square) const noexcept
        -> std::optional<CandidateMoveInfo>;

    [[nodiscard]]
    auto getPiece(Square const &square) const noexcept -> std::optional<Piece>;

    void movePiece(
        Square const &fromSquare,
        Square const &toSquare,
        std::optional<std::string> const &promote);

  private:
    // static constexpr auto indexToSquare(int index) -> Square;
    // static constexpr auto squareToIndex(Square const &square) -> std::size_t;
  };

} // namespace bgg
