// Listener.hpp
#pragma once

#include "Event.hpp"

namespace peeb
{
  ///////////////////////////////////////////////////////////////////////////////
  template <typename EVENT>
    requires EventConcept<EVENT>
  class Listener
  {
  public:
    using Pack = typename EVENT::Pack::ToConstRef::template ToFunction<void>;
    using Variant = typename Pack::template EncloseBy<std::variant>;

  private:
    Variant functor_;

  public:
    Listener() = delete;

    template <typename LISTENER>
      requires(is_in_template<LISTENER, Pack>)
    constexpr Listener(LISTENER const &functor) : functor_(functor)
    {
    }

    /////////////////////////////////////////////////////////////////////////////
    template <typename LISTENER>
      requires(is_in_template_v<LISTENER, Pack>)
    [[nodiscard]] constexpr auto is() const noexcept -> bool
    {
      return std::holds_alternative<LISTENER>(functor_);
    }

    /////////////////////////////////////////////////////////////////////////////
    template <typename LISTENER>
      requires(is_in_template_v<LISTENER, Pack>)
    [[nodiscard]] constexpr auto getIf() const noexcept -> LISTENER const *
    {
      return std::get_if<LISTENER>(&functor_);
    }

    /////////////////////////////////////////////////////////////////////////////
    template <typename LISTENER>
      requires(is_in_template_v<LISTENER, Pack>)
    [[nodiscard]] constexpr auto getIf() noexcept -> LISTENER *
    {
      return std::get_if<LISTENER>(&functor_);
    }

    /////////////////////////////////////////////////////////////////////////////
    template <typename VISITOR>
    // TODO: requires(is_in_template_v<VISITOR, ??? >) -> consider moving `using ListenerVariant` from Bus<> to this template?
    auto visit(VISITOR &&v) const -> decltype(auto)
    {
      return std::visit(std::forward<VISITOR>(v), functor_);
    }
  } // namespace peeb
