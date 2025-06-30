// EventBus.h
#pragma once

#include <tuple>
#include <map>
#include <shared_mutex>
#include <functional>
#include <optional>

#include "PackTransform.hpp"
#include "EventBusHelper.hpp"

#ifdef _DEBUG
#include <iostream>
#endif

namespace peeb // abbr of `Powerful Elegant Event Bus`
{
  template <typename DATA>
  using Listener = std::function<void(DATA const &)>;

  template <typename DATA>
  struct ListenerMapWithMutex
  {
    std::map<std::size_t, Listener<DATA>> listenerMap;
    mutable std::shared_mutex mapMutex;
  };

  // template<typename LISTENTER, typename DATA>
  // concept ListenerConcept = requires(LISTENTER const& listener, DATA const& d)
  //{
  //   { listener(d) } -> std::same_as<void>;
  // };

  ///////////////////////////////////////////////////////////////////////////////
  template <typename... EVENTS>
  class Bus;

  ///////////////////////////////////////////////////////////////////////////////
  /**
   *TODO: Need to generalize to concept of ID Generator(IDGEN).
   *      Because one ID generator may be shared across multi
   *      event buses. Another reason is that we are considering
   *      to implement the Bus<E1, E2...>::unsubscribe(id)
   *      without specifying a concrete En.
   */
  template <EventConcept EVENT>
  class Bus<EVENT> final
  {
  public:
    using Event = EVENT;

    using MapTuple = EVENT::Pack ::template WrapEachIn<ListenerMapWithMutex>::template EncloseBy<std::tuple>;

    using ListenerVariant = EVENT::Pack::ToConstRef ::template ToFunction<void>::template EncloseBy<std::variant>;

  private:
    struct SubscribeVisitor
    {
      Bus *const eventBus;

      template <typename DATA>
        requires(is_in_template_v<DATA, typename EVENT::Pack>)
      decltype(auto) operator()(Listener<DATA> const &e) const
      {
        return eventBus->subscribe<DATA>(e);
      }
    };

    struct UnsubscribeVisitor
    {
      Bus *const eventBus;
      std::size_t const listenerId;

      template <typename DATA>
        requires(is_in_template_v<DATA, typename EVENT::Pack>)
      decltype(auto) operator()(DATA const & /*dummy*/) const
      {
        return eventBus->unsubscribe<DATA>(listenerId);
      }
    };

    struct EmitVisitor
    {
      Bus const *const eventBus;

      template <typename DATA>
        requires(is_in_template_v<DATA, typename EVENT::Pack>)
      decltype(auto) operator()(DATA const &d) const
      {
        return eventBus->emit<DATA>(d);
      }
    };

    // private member variables
    MapTuple listenerMaps_;
    EmitVisitor const emitVisitor_{this};
    SubscribeVisitor const subscribeVisitor_{this};
    std::atomic<std::size_t> nextId_{0};

  public:
    Bus() = default;
    Bus(Bus const &) = delete;
    Bus(Bus &&) = default;

    /////////////////////////////////////////////////////////////////////////////
    template <typename DATA>
      requires(is_in_template_v<DATA, typename EVENT::Pack>)
    void emit(DATA const &e) const noexcept
    {
      auto const &[listenerMap, mutex] = std::get<ListenerMapWithMutex<DATA>>(listenerMaps_);
      std::shared_lock<std::shared_mutex> sharedLock(mutex);
      for (auto const &[id, listener] : listenerMap)
      {
        listener(e);
      }
    }

    /////////////////////////////////////////////////////////////////////////////
    void emit(EVENT const &e) const noexcept
    {
      e.visit(emitVisitor_);
    }

    /////////////////////////////////////////////////////////////////////////////
    // TODO: add static_assert or concept to constraint the type DATA of the below functions
    template <typename DATA>
      requires(is_in_template_v<DATA, typename EVENT::Pack>)
    [[nodiscard]] auto subscribe(Listener<DATA> const &listener) noexcept
        -> std::optional<std::size_t>
    {
      // generate id
      std::size_t id = nextId_++;
      auto &[listenerMap, mutex] = std::get<ListenerMapWithMutex<DATA>>(listenerMaps_);
      std::lock_guard<std::shared_mutex> lock(mutex);
      auto [iter, inserted] = listenerMap.try_emplace(id, listener);
      return inserted ? std::make_optional(id) : std::nullopt;
    }

    /////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] auto subscribe(auto &&listener) noexcept -> decltype(auto)
    {
      if constexpr (std::is_convertible_v<decltype(listener), ListenerVariant>)
        return std::visit(subscribeVisitor_, ListenerVariant(listener));
      else
        return std::optional<std::size_t>(std::nullopt);
    }

    /////////////////////////////////////////////////////////////////////////////
    /**
     *
     * \return number of removed listeners
     */
    template <typename DATA>
      requires(is_in_template_v<DATA, typename EVENT::Pack>)
    auto unsubscribe(std::size_t const &subscriptionId) noexcept -> std::size_t
    {
      auto &[listenerMap, mutex] = std::get<ListenerMapWithMutex<DATA>>(listenerMaps_);
      std::lock_guard<std::shared_mutex> lock(mutex);
      std::size_t removedCount = listenerMap.erase(subscriptionId);
      return removedCount;
    }

    /////////////////////////////////////////////////////////////////////////////
    /**
     * \param dummy The concrete data of EVENT, not the EVENT object ifself.
     * \return number of removed listeners
     */
    auto unsubscribe(EVENT const &dummy, std::size_t const &subscriptionId) noexcept
        -> decltype(auto)
    {
      return dummy.visit(UnsubscribeVisitor{this, subscriptionId});
    }

    /////////////////////////////////////////////////////////////////////////////
    auto unsubscribe(std::size_t const &subscriptionId) noexcept -> std::size_t
    {
      std::size_t removedCount = 0;
      [&]<std::size_t... I>(std::index_sequence<I...>)
      {
        ([&]()
         {
            auto& [listenerMap, mutex] = std::get<I>(listenerMaps_);
            std::lock_guard<std::shared_mutex> lock(mutex);
            removedCount += listenerMap.erase(subscriptionId); }(), ...);
      }(std::make_index_sequence<std::tuple_size_v<MapTuple>>{});

      return removedCount;
    }

    /////////////////////////////////////////////////////////////////////////////
    template <typename DATA>
      requires(is_in_template_v<DATA, typename EVENT::Pack>)
    void clear() noexcept
    {
      auto &[listenerMap, mutex] = std::get<ListenerMapWithMutex<DATA>>(listenerMaps_);
      std::lock_guard<std::shared_mutex> lock(mutex);
      listenerMap.clear();
    }

    /////////////////////////////////////////////////////////////////////////////
    void clearAll() noexcept
    {
      [&]<std::size_t... I>(std::index_sequence<I...>)
      {
        ([&]()
         {
            auto& [listenerMap, mutex] = std::get<I>(listenerMaps_);
            std::lock_guard<std::shared_mutex> lock(mutex);
            listenerMap.clear(); }(), ...);
      }(std::make_index_sequence<std::tuple_size_v<MapTuple>>{});
    }

    /////////////////////////////////////////////////////////////////////////////
    template <typename DATA>
      requires(is_in_template_v<DATA, typename EVENT::Pack>)
    [[nodiscard]] auto hasListeners() const noexcept -> bool
    {
      auto const &[listenerMap, mutex] = std::get<ListenerMapWithMutex<DATA>>(listenerMaps_);
      std::shared_lock<std::shared_mutex> sharedLock(mutex);
      return bool(listenerMap.size());
    }
  };

  ///////////////////////////////////////////////////////////////////////////////
  template <EventConcept E, EventConcept... Es>
  class Bus<E, Es...> final
  {
    using Pack = ppt::Pack<E, Es...>;

    using BusTuple = Pack::template WrapEachIn<Bus>::template EncloseBy<std::tuple>;

    BusTuple childBuses_;

  public:
    Bus() = default;
    Bus(Bus const &) = delete;
    Bus(Bus &&) = default;

    // TODO: What if T is an EventConcept but should be treated as a DATA type?
    // There are 4 cases:
    //    1. T == EVENT && DATA => not handled yet !!!!!!
    //    2. T ==  EVENT && !DATA
    //    3. T == !EVENT &&  DATA
    //    4. T == !EVENT && !DATA => should static_assert
    ///////////////////////////////////////////////////////////////////////////////
    template <EventConcept EVENT, typename DATA>
      requires(
          is_in_template_v<Bus<EVENT>, BusTuple> &&
          is_in_template_v<DATA, typename EVENT::Pack>)
    void emit(DATA const &d) const noexcept
    {
      decltype(auto) childBus = std::get<Bus<EVENT>>(childBuses_);
      childBus.emit<DATA>(d);
    }

    ///////////////////////////////////////////////////////////////////////////////
    template <EventConcept EVENT>
      requires(is_in_template_v<Bus<EVENT>, BusTuple>)
    void emit(EVENT const &e) const noexcept
    {
      decltype(auto) childBus = std::get<Bus<EVENT>>(childBuses_);
      childBus.emit(e);
    }

    /**
     * Usage: emit<
     */
    template <typename DATA>
    // requires (!EventConcept<DATA>)
    void emit(DATA const &d) const noexcept
    {
      [&]<std::size_t... I>(std::index_sequence<I...>)
      {
        ([&]
         {
          using EventType = typename std::tuple_element<I, BusTuple>::type::Event;

          if constexpr (is_in_template_v<DATA, typename EventType::Pack>)
          {
            emit<EventType, DATA>(d);
          }
          else
          {
            //TODO: static_assert( => avoid bloating code
            //  
            //)
          } }(), ...);
      }(std::make_index_sequence<std::tuple_size_v<BusTuple>>{});
    }

    /////////////////////////////////////////////////////////////////////////////
    // TODO: add static_assert or concept to constraint the type DATA and EVENT of the below functions
    template <EventConcept EVENT, typename DATA>
      requires(
          is_in_template_v<Bus<EVENT>, BusTuple> &&
          is_in_template_v<DATA, typename EVENT::Pack>)
    [[nodiscard]] auto subscribe(Listener<DATA> const &listener) noexcept -> decltype(auto)
    {
      decltype(auto) childBus = std::get<Bus<EVENT>>(childBuses_);
      return childBus.subscribe<DATA>(listener);
    }

    /////////////////////////////////////////////////////////////////////////////
    template <EventConcept EVENT>
      requires(is_in_template_v<Bus<EVENT>, BusTuple>)
    [[nodiscard]] auto subscribe(auto &&listener) noexcept -> decltype(auto)
    {
      decltype(auto) childBus = std::get<Bus<EVENT>>(childBuses_);
      return childBus.subscribe(listener);
    }

    /////////////////////////////////////////////////////////////////////////////
    template <typename DATA>
    // requires (!EventConcept<DATA>)
    [[nodiscard]] auto subscribe(Listener<DATA> const &listener) noexcept
        -> std::optional<std::size_t>
    {
      std::optional<std::size_t> returnedId{std::nullopt};

      // TODO: make this function as constexpr_loop instead of copying from above like that
      [&]<std::size_t... I>(std::index_sequence<I...>)
      {
        ([&]
         {
            using EventType = typename std::tuple_element<I, BusTuple>::type::Event;
            if constexpr (is_in_template_v<DATA, typename EventType::Pack>)
            {
              std::optional<std::size_t> id = subscribe<EventType, DATA>(listener);
              if (id)
              {
                returnedId = id;
              }
            } }(), ...);
      }(std::make_index_sequence<std::tuple_size_v<BusTuple>>{});

      return returnedId;
    }

    /////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] auto subscribe(auto &&listener) noexcept -> std::optional<std::size_t> // decltype(auto)
    {
      std::optional<std::size_t> returnedId{std::nullopt};
      // TODO: make this function as constexpr_loop instead of copying from above like that
      [&listener, &returnedId, this]<std::size_t... I>(std::index_sequence<I...>)
      {
        ([&listener, &returnedId, this]
         {
            using EventType = typename std::tuple_element<I, BusTuple>::type::Event;
            std::optional<std::size_t> id = subscribe<EventType>(listener);
            if (id)
            {
              returnedId = id;
            } }(), ...);
      }(std::make_index_sequence<std::tuple_size_v<BusTuple>>{});

      return returnedId;
    }

    /////////////////////////////////////////////////////////////////////////////
    template <EventConcept EVENT, typename DATA>
      requires(is_in_template_v<Bus<EVENT>, BusTuple> &&
               is_in_template_v<DATA, typename EVENT::Pack>)
    auto unsubscribe(std::size_t const &subscriptionId) noexcept -> decltype(auto)
    {
      decltype(auto) childBus = std::get<Bus<EVENT>>(childBuses_);
      return childBus.unsubscribe<DATA>(subscriptionId);
    }

    /////////////////////////////////////////////////////////////////////////////
    template <EventConcept EVENT>
      requires(is_in_template_v<Bus<EVENT>, BusTuple>)
    auto unsubscribe(EVENT const &dummy, std::size_t const &subscriptionId) noexcept -> decltype(auto)
    {
      decltype(auto) childBus = std::get<Bus<EVENT>>(childBuses_);
      return childBus.unsubscribe(dummy, subscriptionId);
    }

    /////////////////////////////////////////////////////////////////////////////
    template <EventConcept EVENT>
      requires(is_in_template_v<Bus<EVENT>, BusTuple>)
    auto unsubscribe(std::size_t const &subscriptionId) noexcept -> decltype(auto)
    {
      decltype(auto) childBus = std::get<Bus<EVENT>>(childBuses_);
      return childBus.unsubscribe(subscriptionId);
    }

    /////////////////////////////////////////////////////////////////////////////
    template <typename DATA>
    auto unsubscribe(std::size_t const &subscriptionId) noexcept -> std::size_t
    {
      std::size_t removedCount = 0;
      [&]<std::size_t... I>(std::index_sequence<I...>)
      {
        ([&]()
         {
            using EventType = typename std::tuple_element<I, BusTuple>::type::Event;
            if constexpr (is_in_template_v<DATA, typename EventType::Pack>)
            {
              removedCount += unsubscribe<EventType, DATA>(subscriptionId);
            } }(), ...);
      }(std::make_index_sequence<std::tuple_size_v<BusTuple>>{});
      return removedCount;
    }

    /////////////////////////////////////////////////////////////////////////////
    auto unsubscribe(std::size_t const &subscriptionId) noexcept -> std::size_t
    {
      std::size_t removedCount = 0;
      [&]<std::size_t... I>(std::index_sequence<I...>)
      {
        ([&]()
         {
            using EventType = typename std::tuple_element<I, BusTuple>::type::Event;
            removedCount += unsubscribe<EventType>(subscriptionId); }(), ...);
      }(std::make_index_sequence<std::tuple_size_v<BusTuple>>{});
      return removedCount;
    }

    /////////////////////////////////////////////////////////////////////////////
    template <EventConcept EVENT, typename DATA>
      requires(is_in_template_v<Bus<EVENT>, BusTuple> &&
               is_in_template_v<DATA, typename EVENT::Pack>)
    void clear() noexcept
    {
      decltype(auto) childBus = std::get<Bus<EVENT>>(childBuses_);
      childBus.clear<DATA>();
    }

    /////////////////////////////////////////////////////////////////////////////
    template <EventConcept EVENT>
      requires(is_in_template_v<Bus<EVENT>, BusTuple>)
    void clear() noexcept
    {
      decltype(auto) childBus = std::get<Bus<EVENT>>(childBuses_);
      childBus.clearAll();
    }

    /////////////////////////////////////////////////////////////////////////////
    template <typename DATA>
    void clear() noexcept
    {
      [&]<std::size_t... I>(std::index_sequence<I...>)
      {
        ([&]()
         {
            using EventType = typename std::tuple_element<I, BusTuple>::type::Event;
            if constexpr (is_in_template_v<DATA, typename EventType::Pack>)
            {
              clear<EventType, DATA>();
            } }(), ...);
      }(std::make_index_sequence<std::tuple_size_v<BusTuple>>{});
    }

    /////////////////////////////////////////////////////////////////////////////
    void clearAll() noexcept
    {
      [&]<std::size_t... I>(std::index_sequence<I...>)
      {
        ([&]()
         {
            using EventType = typename std::tuple_element<I, BusTuple>::type::Event;
            clear<EventType>(); }(), ...);
      }(std::make_index_sequence<std::tuple_size_v<BusTuple>>{});
    }

    /////////////////////////////////////////////////////////////////////////////
    template <EventConcept EVENT, typename DATA>
      requires(is_in_template_v<Bus<EVENT>, BusTuple> &&
               is_in_template_v<DATA, typename EVENT::Pack>)
    auto hasListenters() const noexcept -> bool
    {
      decltype(auto) childBus = std::get<Bus<EVENT>>(childBuses_);
      return childBus.hasListenters<DATA>();
    }

    /////////////////////////////////////////////////////////////////////////////
    template <typename DATA>
    auto hasListenters() const noexcept -> bool
    {
      bool result = false;
      [&]<std::size_t... I>(std::index_sequence<I...>)
      {
        ([&]()
         {
            using EventType = typename std::tuple_element<I, BusTuple>::type::Event;
            if constexpr (is_in_template_v<DATA, typename EventType::Pack>)
            {
              if (hasListenters<EventType, DATA>())
              {
                result = true;
              }
            } }(), ...);
      }(std::make_index_sequence<std::tuple_size_v<BusTuple>>{});
    }

  private:
    // helper functions
  };
} // namespace peeb