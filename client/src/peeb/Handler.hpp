// Handler.hpp
#pragma once

#include <tuple>
#include <functional>

#include "Event.hpp"

#ifdef _DEBUG
#include <iostream>
#endif

namespace peeb
{
  /////////////////////////////////////////////////////////////////////////////
  /**
   *
   * \note This class may be reusable because it is not dependent on any GUI framework
   */
  template <typename EVENT>
    requires(EventConcept<EVENT>)
  class Handler final
  {
    //TODO: Consider replace return type `bool` by a common type RET.
    //      Currently, each handler returns `false` if it does not handle
    //      the input event data. Else it must return true. 
    using Pack = typename EVENT::Pack::ToConstRef::
        template ToFunction<bool>;

    using Tuple = typename Pack::template EncloseBy<std::tuple>;

    Tuple handlers_;

  public:
    Handler() noexcept
    {
      [this]<std::size_t... I>(std::index_sequence<I...>)
      {
        ([this]
         {
          using DataType = typename EVENT::Pack::template At<I>;
          decltype(auto) handler = std::get<I>(handlers_);
          handler = [](DataType const&) -> bool
          { 
            return false;
          }; }(), ...);
      }(std::make_index_sequence<EVENT::Pack::Count>{});
    }

    template <typename DATA>
      requires(is_in_template_v<DATA, typename EVENT::Pack>)
    auto operator()(DATA const &d) const noexcept -> bool
    {
      using HandlerType = std::function<bool(DATA const &)>;
      decltype(auto) handler = std::get<HandlerType>(handlers_);
      return handler(d);
    }

    template <typename DATA>
      requires(is_in_template_v<DATA, typename EVENT::Pack>)
    void setHandler(std::function<bool(DATA const &)> const &newHandler) noexcept
    {
      using HandlerType = std::function<bool(DATA const &)>;
      decltype(auto) handler = std::get<HandlerType>(handlers_);
      handler = newHandler;
    }

    template <typename DATA>
      requires(is_in_template_v<DATA, typename EVENT::Pack>)
    void resetHandler() noexcept
    {
      using HandlerType = std::function<bool(DATA const &)>;
      decltype(auto) handler = std::get<HandlerType>(handlers_);
      handler = [](DATA const &) -> bool
      {
        return false;
      };
    }
  };

} // namespace peeb