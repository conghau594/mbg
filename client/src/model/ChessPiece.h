// ChessPiece.h
#pragma once

#include "base/EnumUtils.h"

namespace bgg
{
#define CHESS_COLOR_ENTRIES(E) \
  E(WHITE)                     \
  E(BLACK)

#define CHESS_COLOR_NAMES(E) \
  E(White)                   \
  E(Black)

  DEFINE_ENUM(ChessColor, CHESS_COLOR_ENTRIES, CHESS_COLOR_NAMES);

#define CHESS_PIECE_ENTRIES(E) \
  E(WHITE_KING)                \
  E(WHITE_QUEEN)               \
  E(WHITE_ROOK_A)              \
  E(WHITE_ROOK_H)              \
  E(WHITE_BISHOP_C)            \
  E(WHITE_BISHOP_F)            \
  E(WHITE_KNIGHT_B)            \
  E(WHITE_KNIGHT_G)            \
  E(WHITE_PAWN_A)              \
  E(WHITE_PAWN_B)              \
  E(WHITE_PAWN_C)              \
  E(WHITE_PAWN_D)              \
  E(WHITE_PAWN_E)              \
  E(WHITE_PAWN_F)              \
  E(WHITE_PAWN_G)              \
  E(WHITE_PAWN_H)              \
  E(BLACK_KING)                \
  E(BLACK_QUEEN)               \
  E(BLACK_ROOK_A)              \
  E(BLACK_ROOK_H)              \
  E(BLACK_BISHOP_C)            \
  E(BLACK_BISHOP_F)            \
  E(BLACK_KNIGHT_B)            \
  E(BLACK_KNIGHT_G)            \
  E(BLACK_PAWN_A)              \
  E(BLACK_PAWN_B)              \
  E(BLACK_PAWN_C)              \
  E(BLACK_PAWN_D)              \
  E(BLACK_PAWN_E)              \
  E(BLACK_PAWN_F)              \
  E(BLACK_PAWN_G)              \
  E(BLACK_PAWN_H)

#define CHESS_PIECE_NAMES(E) \
  E(WhiteKing)               \
  E(WhiteQueen)              \
  E(WhiteRookA)              \
  E(WhiteRookH)              \
  E(WhiteBishopC)            \
  E(WhiteBishopF)            \
  E(WhiteKnightB)            \
  E(WhiteKnightG)            \
  E(WhitePawnA)              \
  E(WhitePawnB)              \
  E(WhitePawnC)              \
  E(WhitePawnD)              \
  E(WhitePawnE)              \
  E(WhitePawnF)              \
  E(WhitePawnG)              \
  E(WhitePawnH)              \
  E(BlackKing)               \
  E(BlackQueen)              \
  E(BlackRookA)              \
  E(BlackRookH)              \
  E(BlackBishopC)            \
  E(BlackBishopF)            \
  E(BlackKnightB)            \
  E(BlackKnightG)            \
  E(BlackPawnA)              \
  E(BlackPawnB)              \
  E(BlackPawnC)              \
  E(BlackPawnD)              \
  E(BlackPawnE)              \
  E(BlackPawnF)              \
  E(BlackPawnG)              \
  E(BlackPawnH)

  DEFINE_ENUM(ChessPiece, CHESS_PIECE_ENTRIES, CHESS_PIECE_NAMES);
}