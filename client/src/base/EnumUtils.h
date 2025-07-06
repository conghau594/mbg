// EnumUtils.h
#pragma once

#include <string>
#include <optional>

#define DEFINE_ENUM(NAME, ENTRY_LIST, NAME_LIST)                              \
  class NAME                                                                  \
  {                                                                           \
  public:                                                                     \
    enum                                                                      \
    {                                                                         \
      ENTRY_LIST(GENERATE_ENUM_ENTRY)                                         \
          __count                                                             \
    };                                                                        \
                                                                              \
    static constexpr int COUNT = static_cast<int>(NAME::__count);             \
    static constexpr const char *NAMES[] = {NAME_LIST(GENERATE_ENUM_STRING)}; \
                                                                              \
    static bool isValid(int value) noexcept                                   \
    {                                                                         \
      return value >= 0 && value < COUNT;                                     \
    }                                                                         \
                                                                              \
    static std::optional<std::string> toString(int value) noexcept            \
    {                                                                         \
      if (isValid(value))                                                     \
      {                                                                       \
        return NAMES[value];                                                  \
      }                                                                       \
      return std::nullopt;                                                    \
    }                                                                         \
                                                                              \
    static std::optional<int> fromString(const std::string &str) noexcept     \
    {                                                                         \
      for (int i = 0; i < COUNT; ++i)                                         \
      {                                                                       \
        if (NAMES[i] == str)                                                  \
        {                                                                     \
          return i;                                                           \
        }                                                                     \
      }                                                                       \
      return std::nullopt;                                                    \
    }                                                                         \
  }

#define GENERATE_ENUM_ENTRY(name) name,
#define GENERATE_ENUM_STRING(name) #name,
