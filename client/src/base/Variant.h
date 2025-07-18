// Event.hpp
#pragma once

#include <variant>

#include "peeb/PackTransform.hpp"
#include "peeb/ConceptHelper.hpp"

namespace bgg
{
  ///////////////////////////////////////////////////////////////////////////////
  template <typename... T>
  class Variant final
  {
  public:
    using Pack = ppt::Pack<std::monostate, T...>;

  private:
    Pack::template EncloseBy<std::variant> data_;

  public:
    constexpr Variant() noexcept = default;

    template <typename U>
      requires(peeb::is_in_template_v<U, Pack>)
    constexpr Variant(U const &data) noexcept : data_(data)
    {
    }

    /////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] constexpr auto getIndex() const noexcept -> std::size_t
    {
      return data_.index();
    }

    /////////////////////////////////////////////////////////////////////////////
    template <typename U>
      requires(peeb::is_in_template_v<U, Pack>)
    [[nodiscard]] constexpr auto is() const noexcept -> bool
    {
      return std::holds_alternative<U>(data_);
    }

    /////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] constexpr auto isEmpty() const noexcept -> bool
    {
      return std::holds_alternative<std::monostate>(data_);
    }

    /////////////////////////////////////////////////////////////////////////////
    constexpr void setEmpty() noexcept
    {
      data_ = std::monostate{};
    }

    /////////////////////////////////////////////////////////////////////////////
    template <typename U>
      requires(peeb::is_in_template_v<U, Pack>)
    [[nodiscard]] constexpr auto getIf() const noexcept -> U const *
    {
      return std::get_if<U>(&data_);
    }

    /////////////////////////////////////////////////////////////////////////////
    template <typename U>
      requires(peeb::is_in_template_v<U, Pack>)
    [[nodiscard]] constexpr auto getIf() noexcept -> U *
    {
      return std::get_if<U>(&data_);
    }

    /////////////////////////////////////////////////////////////////////////////
    template <typename VISITOR>
    // TODO: requires(is_in_template_v<VISITOR, ??? >) -> consider moving `using ListenerVariant` from Bus<> to this template?
    auto visit(VISITOR &&func) const -> decltype(auto)
    {
      return std::visit(std::forward<VISITOR>(func), data_);
    }

    /////////////////////////////////////////////////////////////////////////////
    template <typename VISITOR>
    // TODO: requires(is_in_template_v<VISITOR, ??? >) -> consider moving `using ListenerVariant` from Bus<> to this template?
    auto visit(VISITOR &&func) -> decltype(auto)
    {
      return std::visit(std::forward<VISITOR>(func), data_);
    }
  };
} // namespace peeb