// ErrorCode.h
#pragma once

#include <string>

namespace bgg
{
  class ErrorCode
  {
  public:
    int const value;
    std::string const category;
    std::string const message;

    ErrorCode(ErrorCode const &) noexcept = default;

    ErrorCode(
        int errValue,
        std::string const &errCategory,
        std::string const &errMsg) noexcept
        : value(errValue), category(errCategory), message(errMsg)
    {
    }

    template <typename Enum>
    ErrorCode(
        Enum errValue,
        std::string const &errMsg) noexcept
        : value(errValue), category(typeid(Enum).name()), message(errMsg)
    {
    }

    bool operator==(const ErrorCode &other) const noexcept
    {
      return (value == other.value) && (category == other.category);
    }

    bool failed() const noexcept { return value != 0; }
  };
}