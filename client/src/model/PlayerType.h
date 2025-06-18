// PlayerType.h
#pragma once

#include <string>
#include <optional>

namespace iab
{
  class PlayerType final
  {
    inline static std::vector<std::string> const PLAYER_TYPE_STRINGS{
        "Human",
        "ChatGPT",
        "Gemini"};

  public:
    enum
    {
      HUMAN,
      CHATGPT,
      GEMINI,
    };

    static bool isValid(int playerType) noexcept
    {
      return playerType >= 0 && playerType < PLAYER_TYPE_STRINGS.size();
    }

    static std::optional<std::string> toString(int playerType) noexcept
    {
      if (isValid(playerType))
      {
        return PLAYER_TYPE_STRINGS[playerType];
      }
      return std::nullopt;
    }

    static std::optional<int> fromString(const std::string &playerTypeStr) noexcept
    {
      for (int i = 0; i < (const int)PLAYER_TYPE_STRINGS.size(); ++i)
      {
        if (PLAYER_TYPE_STRINGS[i] == playerTypeStr)
        {
          return i;
        }
      }

      return std::nullopt; // Unknown player type
    }
  };
}