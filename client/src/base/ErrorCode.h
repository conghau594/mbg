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

    [[nodiscard]]
    auto operator==(const ErrorCode &other) const noexcept -> bool
    {
      return (value == other.value) && (category == other.category);
    }

    [[nodiscard]]
    auto failed() const noexcept -> bool
    {
      return value != 0;
    }
  };
}