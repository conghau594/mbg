// ChessUtils.h
#pragma once

#include <boost/assert.hpp>
#include "model/Piece.h"

#define BGG_VALIDATE_CHESS_PIECE(type) BOOST_ASSERT_MSG(              \
    (type == ChessUtils::KING) || (type == ChessUtils::QUEEN) ||      \
        (type == ChessUtils::ROOK) || (type == ChessUtils::BISHOP) || \
        (type == ChessUtils::KNIGHT) || (type == ChessUtils::PAWN),   \
    "Type of chess piece must be one of 'King', 'Queen', 'Rook', 'Bishop', 'Knight', 'Pawn'")

#define BGG_VALIDATE_COLOR(color) BOOST_ASSERT_MSG(     \
    (color == Color::WHITE) || (color == Color::BLACK), \
    "Player side must be Color::WHITE or Color::BLACK")

#define BGG_VALIDATE_SQUARE(square) BOOST_ASSERT_MSG(                             \
    square[0] >= 'a' && square[0] <= 'h' && square[1] >= '1' && square[1] <= '8', \
    "Position index must be from 'a1' to 'h8'")

namespace bgg
{
  class ChessUtils
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

    static inline std::pair<Position, Piece> INITIAL_PLACEMENTS[PIECE_COUNT]{
        {Position{"e1"}, Piece{KING, Color::WHITE}},
        {Position{"d1"}, Piece{QUEEN, Color::WHITE}},
        {Position{"a1"}, Piece{ROOK, Color::WHITE}},
        {Position{"h1"}, Piece{ROOK, Color::WHITE}},
        {Position{"b1"}, Piece{KNIGHT, Color::WHITE}},
        {Position{"g1"}, Piece{KNIGHT, Color::WHITE}},
        {Position{"c1"}, Piece{BISHOP, Color::WHITE}},
        {Position{"f1"}, Piece{BISHOP, Color::WHITE}},
        {Position{"a2"}, Piece{PAWN, Color::WHITE}},
        {Position{"b2"}, Piece{PAWN, Color::WHITE}},
        {Position{"c2"}, Piece{PAWN, Color::WHITE}},
        {Position{"d2"}, Piece{PAWN, Color::WHITE}},
        {Position{"e2"}, Piece{PAWN, Color::WHITE}},
        {Position{"f2"}, Piece{PAWN, Color::WHITE}},
        {Position{"g2"}, Piece{PAWN, Color::WHITE}},
        {Position{"h2"}, Piece{PAWN, Color::WHITE}},

        {Position{"e8"}, Piece{KING, Color::BLACK}},
        {Position{"d8"}, Piece{QUEEN, Color::BLACK}},
        {Position{"a8"}, Piece{ROOK, Color::BLACK}},
        {Position{"h8"}, Piece{ROOK, Color::BLACK}},
        {Position{"b8"}, Piece{KNIGHT, Color::BLACK}},
        {Position{"g8"}, Piece{KNIGHT, Color::BLACK}},
        {Position{"c8"}, Piece{BISHOP, Color::BLACK}},
        {Position{"f8"}, Piece{BISHOP, Color::BLACK}},
        {Position{"a7"}, Piece{PAWN, Color::BLACK}},
        {Position{"b7"}, Piece{PAWN, Color::BLACK}},
        {Position{"c7"}, Piece{PAWN, Color::BLACK}},
        {Position{"d7"}, Piece{PAWN, Color::BLACK}},
        {Position{"e7"}, Piece{PAWN, Color::BLACK}},
        {Position{"f7"}, Piece{PAWN, Color::BLACK}},
        {Position{"g7"}, Piece{PAWN, Color::BLACK}},
        {Position{"h7"}, Piece{PAWN, Color::BLACK}},
    };
  };
}