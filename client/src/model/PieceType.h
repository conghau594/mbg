// PieceType.h
#pragma once

#include "base/EnumUtils.h"

namespace iab
{
#define WESTERN_PIECE_TYPE_ENTRIES(E) \
  E(KING)                             \
  E(QUEEN)                            \
  E(ROOK)                             \
  E(BISHOP)                           \
  E(KNIGHT)                           \
  E(PAWN)

#define WESTERN_PIECE_TYPE_NAMES(E) \
  E(King)                           \
  E(Queen)                          \
  E(Rook)                           \
  E(Bishop)                         \
  E(Knight)                         \
  E(Pawn)

  DEFINE_ENUM(WesternPiece, WESTERN_PIECE_TYPE_ENTRIES, WESTERN_PIECE_TYPE_NAMES);
}