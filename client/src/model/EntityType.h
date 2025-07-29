// EntityType.h
#pragma once

#include <array>
#include <string>

#include <boost/assert.hpp>

namespace bgg
{
  class EntityType
  {
    std::array<char, 16> name_;

  public:
    constexpr EntityType() = default;
    constexpr EntityType(std::string_view name) noexcept
        : name_{}
    {
      BOOST_ASSERT_MSG(
          name.size() < name_.size(),
          "EntityType name must be less than 16 characters long.");

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

    // void operator=(EntityType const &other) noexcept = default;

    [[nodiscard]] auto toString() const noexcept -> std::string
    {
      return std::string(name_.data());
    }

    [[nodiscard]] constexpr auto operator==(
        const EntityType &other) const noexcept -> bool
    {
      return name_ == other.name_;
    }
  };
} // namespace bgg
