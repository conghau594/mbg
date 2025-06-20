// GameType.h
#pragma once

#include "base/EnumUtils.h"

namespace iab
{

#define GAME_TYPE_ENTRIES(E) \
  E(WESTERN_CHESS)           \
  E(CHINESE_CHESS)           \
  E(TIC_TAC_TOE)             \
  E(GOMOKU)

#define GAME_TYPE_NAMES(E) \
  E(Western Chess)         \
  E(Chinese Chess)         \
  E(Tic Tac Toe)           \
  E(Gomoku)

  DEFINE_ENUM(GameType, GAME_TYPE_ENTRIES, GAME_TYPE_NAMES);
}