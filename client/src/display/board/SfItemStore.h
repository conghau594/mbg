// SfItemStore.h
#pragma once

#include <map>
#include <atomic>
#include <string>
#include <variant>
#include <memory>

#include <SFML/System/Vector2.hpp>

#include "SfBoardItem.h"

namespace bgg
{
  class SfItemStore final
  {
    using Container = std::map<std::size_t, SfBoardItem>;

    Container boardItems_;
    std::shared_ptr<const SfTextureAtlas> const itemTextureAtlas_;
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
    // SfItemStore(SfItemStore &&other) noexcept;

    SfItemStore(std::shared_ptr<const SfTextureAtlas> itemTextureAtlas) noexcept;

    auto addItem(
        int textureCellIndex,
        int zOrder,
        std::string name = "",
        bool visible = true) noexcept -> Entry;

    // auto removeItem(std::size_t itemId) noexcept -> bool;

    auto getZOrder(Entry const &entry) const noexcept -> int;
    void changeZOrder(Entry &entry, int newZOrder) noexcept;
    auto removeItem(Entry &entry) noexcept -> bool;

    [[nodiscard]] auto begin() noexcept -> Iter;
    [[nodiscard]] auto end() noexcept -> Iter;

    [[nodiscard]] auto begin() const noexcept -> Iter;
    [[nodiscard]] auto end() const noexcept -> Iter;

  private:
    [[nodiscard]] auto generateNextId(int zOrder) noexcept -> std::size_t;
  };

  /////////////////////////////////////////////////////////////////////////////
  class SfItemStore::Iter final
  {
    friend class SfItemStore;

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
    Iter(SfItemStore const &itemStore) noexcept;

    void operator++() noexcept;
    void operator++() const noexcept;
    [[nodiscard]] auto operator*() noexcept -> SfBoardItem &;
    [[nodiscard]] auto operator*() const noexcept -> SfBoardItem const &;
    [[nodiscard]] auto operator==(Iter const &rhs) const noexcept -> bool;

    void *operator new(std::size_t) = delete;
    void operator delete(void *) = delete;
  };
  /////////////////////////////////////////////////////////////////////////////
  class SfItemStore::Entry final
  {
    friend class SfItemStore;

    std::shared_ptr<Container::iterator> itemIter_;
    Container const *itemMapPtr_;

    Entry(
        std::shared_ptr<Container::iterator> itemIter,
        Container const *itemMapPtr) noexcept;

  public:
    /**
     * Construct an null Entry object associated with an ItemStore
     *
     */
    Entry(SfItemStore const &itemStore) noexcept;

    /**
     * \return True if two entries are copies of each other. Otherwise false.
     *
     * \note Two entries cannot refer to one std::map::iterator except they
     *       are copies of each other.
     */
    [[nodiscard]] auto operator==(Entry const &rhs) const noexcept -> bool;

    /**
     * An Entry object is null when and only when it's removed from its SfItemStore.
     */
    [[nodiscard]] auto isNull() const noexcept -> bool;

    /**
     * An assertion raises if the Entry object is null
     *
     */
    [[nodiscard]] auto getItem() noexcept -> SfBoardItem &;
  };
} // namespace bgg
