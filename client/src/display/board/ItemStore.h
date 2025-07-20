// ItemStore.h
#pragma once

#include <map>
#include <atomic>
#include <string>
#include <variant>
#include <functional>
#include <memory>

#include <SFML/System/Vector2.hpp>

#include "BoardItem.h"

namespace bgg
{
  class ItemStore final
  {
    using Container = std::map<std::size_t, BoardItem>;

    Container boardItems_;
    std::shared_ptr<const BaseTextureAtlas> const itemTextureAtlas_;
    std::atomic<std::size_t> nextBaseItemId_; ///< Id generator for the boardItems_ (std::map )

    constexpr static int Z_ORDER_BIT_COUNT = 8;
    constexpr static int MAX_Z_ORDER = 1 << Z_ORDER_BIT_COUNT;

  public:
    class Iter;
    class Entry;

    // !!! IMPORTANT:
    //
    // This class cannot move-construct not due to the std::atomic variable
    // but the change of iterator when use std::move(std::map). There is
    // no problem when compiling with GCC but with MSVC, you got an assertion
    // of "map/set iterators incompatible".
    //
    // ItemStore(ItemStore &&other) noexcept;

    ItemStore(std::shared_ptr<const BaseTextureAtlas> itemTextureAtlas) noexcept;

    auto addItem(
        int zOrder,
        int textureCellIndex,
        std::string name = "",
        bool visible = true) noexcept -> Entry;

    auto addItem(int zOrder, BoardItem item) noexcept -> Entry;

    auto createItem(
        int textureCellIndex,
        std::string name = "",
        bool visible = true) noexcept -> BoardItem;

    auto findItems(
        std::function<bool(BoardItem const &item)> const &predicate) noexcept
        -> std::list<BoardItem *>;

    // auto removeItem(std::size_t itemId) noexcept -> bool;
    auto removeItem(Entry &entry) noexcept -> bool;

    auto getZOrder(Entry const &entry) const noexcept -> int;
    void changeZOrder(Entry &entry, int newZOrder) noexcept;
    // void changeItem(
    //     Entry &entry,
    //     int textureCellIndex,
    //     std::string name = "",
    //     bool visible = true) noexcept;

    [[nodiscard]] auto begin() noexcept -> Iter;
    [[nodiscard]] auto end() noexcept -> Iter;

    [[nodiscard]] auto begin() const noexcept -> Iter;
    [[nodiscard]] auto end() const noexcept -> Iter;

  private:
    [[nodiscard]] auto generateNextId(int zOrder) noexcept -> std::size_t;
  };

  /////////////////////////////////////////////////////////////////////////////
  class ItemStore::Iter final
  {
    friend class ItemStore;

    mutable std::variant<Container::iterator, Container::const_iterator> itemIter_;
    Container const *const itemMapPtr_;

    Iter(
        std::variant<Container::iterator, Container::const_iterator> itemIter,
        Container const *const itemMapPtr) noexcept;

  public:
    /**
     * Construct an null Entry object associated with an ItemStore
     *
     */
    Iter(ItemStore const &itemStore) noexcept;

    void operator++() noexcept;
    void operator++() const noexcept;
    [[nodiscard]] auto operator*() noexcept -> BoardItem &;
    [[nodiscard]] auto operator*() const noexcept -> BoardItem const &;
    [[nodiscard]] auto operator==(Iter const &rhs) const noexcept -> bool;

    void *operator new(std::size_t) = delete;
    void operator delete(void *) = delete;
  };
  /////////////////////////////////////////////////////////////////////////////
  class ItemStore::Entry final
  {
    friend class ItemStore;

    std::shared_ptr<Container::iterator> itemIter_;
    Container const *itemMapPtr_;

    Entry(std::shared_ptr<Container::iterator> itemIter,
          Container const *itemMapPtr) noexcept;

  public:
    /**
     * Construct an null Entry object associated with an ItemStore
     *
     */
    constexpr Entry() noexcept : itemIter_(nullptr), itemMapPtr_(nullptr)
    {
    }

    /**
     * \return True if two entries are copies of each other. Otherwise false.
     *
     * \note Two entries cannot refer to one std::map::iterator except they
     *       are copies of each other.
     */
    [[nodiscard]] auto operator==(Entry const &rhs) const noexcept -> bool;

    /**
     * An Entry object is null when and only when it's removed from its ItemStore.
     */
    [[nodiscard]] auto isNull() const noexcept -> bool;

    /**
     * An assertion raises if the Entry object is null
     *
     */
    [[nodiscard]] auto getItem() const noexcept -> BoardItem &;
  };
} // namespace bgg
