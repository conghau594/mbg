// EnumBitwises.h
#pragma once

#include <type_traits>

namespace bgg
{

  template <typename E>
  inline E operator|(E const &lhs, E const &rhs)
  {
    using T = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<T>(lhs) | static_cast<T>(rhs));
  }

  // Bitwise AND
  template <typename E>
  inline E operator&(E const &lhs, E const &rhs)
  {
    using T = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<T>(lhs) & static_cast<T>(rhs));
  }

  // Compound OR
  template <typename E>
  inline E &operator|=(E &lhs, E const &rhs)
  {
    lhs = lhs | rhs;
    return lhs;
  }

  // Compound AND
  template <typename E>
  inline E &operator&=(E &lhs, E const &rhs)
  {
    lhs = lhs & rhs;
    return lhs;
  }

  // Inversion (optional)
  template <typename E>
  inline E operator~(E const &e)
  {
    using T = std::underlying_type_t<E>;
    return static_cast<E>(~static_cast<T>(e));
  }

} // namespace bgg
