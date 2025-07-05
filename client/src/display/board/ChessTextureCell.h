// ChessBoardItem.h
#pragma once

namespace bgg
{
  enum ChessTextureCell
  {
    WHITE_SQUARE,
    BLACK_SQUARE,

    // white pieces
    WHITE_KING,
    WHITE_QUEEN,
    WHITE_ROOK,
    WHITE_BISHOP,
    WHITE_KNIGHT,
    WHITE_PAWN,

    // black pieces
    BLACK_KING,
    BLACK_QUEEN,
    BLACK_ROOK,
    BLACK_BISHOP,
    BLACK_KNIGHT,
    BLACK_PAWN,

    // highlighter
    CHOICE_HIGHLIGHTER,
    LAST_MOVE_HIGHLIGHTER,
    QUIET_MOVE_HIGHLIGHTER,
    ATTACK_MOVE_HIGHLIGHTER,
    CHECK_HIGHLIGHTER,
    CHECKMATE_HIGHLIGHTER,
  };

} // namespace bgg
