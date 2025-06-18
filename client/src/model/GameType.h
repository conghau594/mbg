// GameType.h
#pragma once

#include <string>
#include <optional>

namespace iab
{
  class GameType final
  {
    inline static std::vector<std::string> const GAME_TYPE_STRINGS{
        "Western Chess",
        "Chinese Chess"};

  public:
    enum
    {
      WESTERN_CHESS,
      CHINESE_CHESS,
    };

    static bool isValid(int gameType) noexcept
    {
      return gameType >= 0 && gameType < GAME_TYPE_STRINGS.size();
    }

    static std::optional<std::string> toString(int gameType) noexcept
    {
      if (isValid(gameType))
      {
        return GAME_TYPE_STRINGS[gameType];
      }

      return std::nullopt;
    }

    static std::optional<int> fromString(const std::string &gameTypeStr) noexcept
    {
      for (int i = 0; i < (const int)GAME_TYPE_STRINGS.size(); ++i)
      {
        if (GAME_TYPE_STRINGS[i] == gameTypeStr)
        {
          return i;
        }
      }

      return std::nullopt; // Unknown game type
    }
  };
}