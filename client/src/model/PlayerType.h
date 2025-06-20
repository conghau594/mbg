// PlayerType.h
#pragma once

#include "base/EnumUtils.h"

namespace iab
{
#define PLAYER_TYPE_ENTRIES(E) \
  E(HUMAN)                     \
  E(CHATGPT)                   \
  E(GEMINI)                    \
  E(CLAUDE)

#define PLAYER_TYPE_NAMES(E) \
  E(Human)                   \
  E(ChatGPT)                 \
  E(Gemini)                  \
  E(Claude)

  DEFINE_ENUM(PlayerType, PLAYER_TYPE_ENTRIES, PLAYER_TYPE_NAMES);
}