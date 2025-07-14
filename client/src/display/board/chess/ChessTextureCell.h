// ChessBoardItem.h
#pragma once

#include "base/EnumUtils.h"
namespace bgg
{

#define CHESS_TEXTURE_CELL_ENTRIES(E) \
  E(WHITE_SQUARE)                     \
  E(BLACK_SQUARE)                     \
  E(WHITE_KING)                       \
  E(WHITE_QUEEN)                      \
  E(WHITE_ROOK)                       \
  E(WHITE_BISHOP)                     \
  E(WHITE_KNIGHT)                     \
  E(WHITE_PAWN)                       \
  E(BLACK_KING)                       \
  E(BLACK_QUEEN)                      \
  E(BLACK_ROOK)                       \
  E(BLACK_BISHOP)                     \
  E(BLACK_KNIGHT)                     \
  E(BLACK_PAWN)                       \
  E(CHOICE_HIGHLIGHTER)               \
  E(LAST_MOVE_HIGHLIGHTER)            \
  E(QUIET_MOVE_HIGHLIGHTER)           \
  E(CAPTURE_MOVE_HIGHLIGHTER)         \
  E(CHECK_HIGHLIGHTER)                \
  E(CHECKMATE_HIGHLIGHTER)

#define CHESS_TEXTURE_CELL_NAMES(E) \
  E(WhiteSquare)                    \
  E(BlackSquare)                    \
  E(WhiteKing)                      \
  E(WhiteQueen)                     \
  E(WhiteRook)                      \
  E(WhiteBishop)                    \
  E(WhiteKnight)                    \
  E(WhitePawn)                      \
  E(BlackKing)                      \
  E(BlackQueen)                     \
  E(BlackRook)                      \
  E(BlackBishop)                    \
  E(BlackKnight)                    \
  E(BlackPawn)                      \
  E(ChoiceHighlighter)              \
  E(LastMoveHighlighter)            \
  E(QuietMoveHighlighter)           \
  E(CaptureMoveHighlighter)         \
  E(CheckHighlighter)               \
  E(CheckmateHighlighter)

  DEFINE_ENUM(ChessTextureCell, CHESS_TEXTURE_CELL_ENTRIES, CHESS_TEXTURE_CELL_NAMES);
} // namespace bgg
