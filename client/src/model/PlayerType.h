// PlayerType.h
#pragma once

#include "base/EnumUtils.h"

namespace bgg
{
#define PLAYER_TYPE_ENTRIES(E) \
  E(GEMINI)
  // E(HUMAN)
  // E(CHATGPT)
  // E(CLAUDE)

#define PLAYER_TYPE_NAMES(E) \
  E(Gemini)
  // E(Human)
  // E(ChatGPT)
  // E(Claude)

  DEFINE_ENUM(PlayerType, PLAYER_TYPE_ENTRIES, PLAYER_TYPE_NAMES);
}