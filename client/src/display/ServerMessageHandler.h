// ServerMessageHandler.h
#pragma once

#include "service/ServerMessage.h"

#ifdef _DEBUG
#include <iostream>
#endif

namespace bgg
{
  /////////////////////////////////////////////////////////////////////////////
  /**
   *
   * \note This class may be reusable because it is not dependent on any GUI framework
   */
  class ServerMessageHandler
  {
    using HandlerTuples = ServerMessage::Pack::ToConstRef ::template ToFunction<bool>::template EncloseBy<std::tuple>;

    HandlerTuples handlers_;

  public:
    ServerMessageHandler() noexcept
    {
      [this]<std::size_t... I>(std::index_sequence<I...>)
      {
        ([this]
         {
          using DataType = ServerMessage::Pack::At<I>;
          decltype(auto) handler = std::get<I>(handlers_);
          handler = [](DataType const&) -> bool
          { 
            return false;
          }; }(), ...);
      }(std::make_index_sequence<ServerMessage::Pack::Count>{});
    }

    template <typename DATA>
      requires(peeb::is_in_template_v<DATA, ServerMessage::Pack>)
    auto operator()(DATA const &d) const noexcept -> decltype(auto)
    {
      using HandlerType = std::function<bool(DATA const &)>;
      decltype(auto) handler = std::get<HandlerType>(handlers_);
      bool isHandled = handler(d);

#ifdef _DEBUG
      if (isHandled)
      {
        std::clog << "\nSfBaseScreen has handled ServerMessage of "
                  << typeid(DATA).name();
      }
      // else
      // {
      //   std::clog << "\nSfBaseScreen has ignored ServerMessage of "
      //             << typeid(DATA).name();
      // }
#endif
      return isHandled;
    }

    template <typename DATA>
      requires(peeb::is_in_template_v<DATA, ServerMessage::Pack>)
    void setHandler(std::function<bool(DATA const &)> const &newHandler) noexcept
    {
      using HandlerType = std::function<bool(DATA const &)>;
      decltype(auto) handler = std::get<HandlerType>(handlers_);
      handler = newHandler;
    }

    template <typename DATA>
      requires(peeb::is_in_template_v<DATA, ServerMessage::Pack>)
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

}