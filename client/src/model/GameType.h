// GameType.h
#pragma once

#include "base/EnumUtils.h"

namespace bgg
{

#define GAME_TYPE_ENTRIES(E) \
  E(WESTERN_CHESS)           \
  E(GOMOKU)                  \
  // E(CHINESE_CHESS)
  // E(TIC_TAC_TOE)

#define GAME_TYPE_NAMES(E) \
  E(Western Chess)         \
  E(Gomoku)                \
  // E(Chinese Chess)
  // E(Tic Tac Toe)

  DEFINE_ENUM(GameType, GAME_TYPE_ENTRIES, GAME_TYPE_NAMES);
}