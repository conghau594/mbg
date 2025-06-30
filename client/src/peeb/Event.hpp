// ClientEvent.h
#pragma once

#include <string>
#include <variant>

#include "PackTransform.hpp"
#include "EventBusHelper.hpp"

namespace peeb // abbr of `Powerful Elegant Event Bus`
{
  ///////////////////////////////////////////////////////////////////////////////
  template <typename EVENT>
  concept EventConcept = requires(EVENT const &e) {
    typename EVENT::Pack;
    {
      e.visit([](auto &&) {})
    };
  };

  ///////////////////////////////////////////////////////////////////////////////
  template <typename... DATA>
  class Event final
  {
  public:
    using Pack = ppt::Pack<DATA...>;
    using Variant = Pack::template EncloseBy<std::variant>;

  private:
    Variant data_;

  public:
    Event() = delete;
    ////Method 1: auto (implicit) template
    // Event(auto const& data) : data_(data) { }

    // decltype(auto) visit(auto&& handler)
    //{
    //   return std::visit(std::forward<decltype(handler)>(handler), data_);
    // }

    // Method 2: explicit template
    template <typename SUBDATA>
      requires(is_in_template_v<SUBDATA, Pack>)
    constexpr Event(SUBDATA const &data) : data_(data)
    {
    }

    /////////////////////////////////////////////////////////////////////////////
    template <typename SUBDATA>
      requires(is_in_template_v<SUBDATA, Pack>)
    [[nodiscard]] constexpr auto is() const noexcept -> bool
    {
      return std::holds_alternative<SUBDATA>(data_);
    }

    /////////////////////////////////////////////////////////////////////////////
    template <typename SUBDATA>
      requires(is_in_template_v<SUBDATA, Pack>)
    [[nodiscard]] constexpr auto getIf() const noexcept -> SUBDATA const *
    {
      return std::get_if<SUBDATA>(&data_);
    }

    /////////////////////////////////////////////////////////////////////////////
    template <typename SUBDATA>
      requires(is_in_template_v<SUBDATA, Pack>)
    [[nodiscard]] constexpr auto getIf() noexcept -> SUBDATA *
    {
      return std::get_if<SUBDATA>(&data_);
    }

    /////////////////////////////////////////////////////////////////////////////
    template <typename VISITOR>
    // TODO: requires(is_in_template_v<VISITOR, ??? >) -> consider moving `using ListenerVariant` from Bus<> to this template?
    auto visit(VISITOR &&func) const -> decltype(auto)
    {
      return std::visit(std::forward<VISITOR>(func), data_);
    }
  };
} // namespace peeb