// Side.h
#pragma once

#include <array>
#include <string>

#include <boost/assert.hpp>

namespace bgg
{
  class Side
  {
    std::array<char, 16> name_;

  public:
    enum class Status : unsigned
    {
      UNDEFINED,
      SAFE,
      IN_CHECK,
      CHECKMATED,
      STALEMATED
    };

    constexpr Side() = default;

    constexpr Side(std::string_view name) noexcept
        : name_{}
    {
      BOOST_ASSERT_MSG(
          name.size() < name_.size(),
          "Side name must be less than 16 characters long.");

      std::copy_n(name.begin(), name.size(), name_.begin());
      name_[name.size()] = '\0'; // Ensure null-termination
    }

    void operator=(std::string_view name) noexcept
    {
      BOOST_ASSERT_MSG(
          name.size() < name_.size(),
          "Side name must be less than 16 characters long.");

      std::copy_n(name.begin(), name.size(), name_.begin());
      name_[name.size()] = '\0'; // Ensure null-termination
    }

    // void operator=(Side const &other) noexcept = default;

    [[nodiscard]] auto toString() const noexcept -> std::string
    {
      return std::string(name_.data());
    }

    [[nodiscard]] constexpr auto operator==(
        const Side &other) const noexcept -> bool
    {
      return name_ == other.name_;
    }

    [[nodiscard]] static auto toString(Status sideStatus) noexcept -> std::string
    {
      switch (sideStatus)
      {
      case Status::UNDEFINED:
        return "Undefined";

      case Status::SAFE:
        return "Safe";

      case Status::IN_CHECK:
        return "In check";

      case Status::CHECKMATED:
        return "Checkmated";

      default:
        BOOST_ASSERT_MSG(false, "Invalid 'Side::Status' value.");
        return "";
      }
    }
  };
} // namespace bgg
