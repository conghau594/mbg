// GameType.h
#pragma once

#include "base/EnumUtils.h"

namespace bgg
{

#define GAME_TYPE_ENTRIES(E) \
  E(CHESS)
  // E(GOMOKU)
  // E(XIANGQI) \
  // E(TIC_TAC_TOE)

#define GAME_TYPE_NAMES(E) \
  E(Chess)
  // E(Gomoku)
  //  E(Xiangqi)
  //  E(Tic Tac Toe)

  DEFINE_ENUM(GameType, GAME_TYPE_ENTRIES, GAME_TYPE_NAMES);
}