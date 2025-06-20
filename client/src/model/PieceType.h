// PieceType.h
#pragma once

#include "base/EnumUtils.h"

namespace iab
{
#define PIECE_TYPE_ENTRIES(E) \
  E(KING)                     \
  E(QUEEN)                    \
  E(ROOK)                     \
  E(BISHOP)                   \
  E(KNIGHT)                   \
  E(PAWN)

#define PIECE_TYPE_NAMES(E) \
  E(KING)                   \
  E(QUEEN)                  \
  E(ROOK)                   \
  E(BISHOP)                 \
  E(KNIGHT)                 \
  E(PAWN)

  DEFINE_ENUM(PieceType, PIECE_TYPE_ENTRIES, PIECE_TYPE_NAMES);
}