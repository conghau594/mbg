// SfItemStore.h
#pragma once

#include <map>
#include <atomic>
#include <string>
#include <optional>
#include <memory>

#include <SFML/System/Vector2.hpp>

#include "SfBoardItem.h"

namespace bgg
{
  class SfItemStore final
  {
    std::shared_ptr<const SfTextureAtlas> const itemTextureAtlas_;

    mutable std::map<std::size_t, SfBoardItem> boardItems_;
    std::atomic<std::size_t> nextBaseItemId_; ///< Id generator for the boardItems_ (std::map )

    constexpr static int Z_ORDER_BIT_COUNT = 8;
    constexpr static int MAX_Z_ORDER = 1 << Z_ORDER_BIT_COUNT;

  public:
    class Entry;

    // !!! IMPORTANT:
    //
    // This class cannot move-construct not due to the std::atomic variable
    // but the change of iterator when use std::move(std::map). There is
    // no problem when compiling with GCC but with MSVC, I got the assert
    // of "map/set iterators incompatible".
    //
    // SfItemStore(SfItemStore &&other) noexcept;

    SfItemStore(std::shared_ptr<const SfTextureAtlas> itemTextureAtlas) noexcept;

    // auto addItem(SfBoardItem item, int zOrder, bool visible) noexcept
    //     -> std::optional<Entry>;

    auto addItem(
        int textureCellIndex,
        int zOrder,
        std::string name = "",
        bool visible = true) noexcept -> Entry;

    // auto removeItem(std::size_t itemId) noexcept -> bool;

    auto getZOrder(Entry const &entry) const noexcept -> int;
    void changeZOrder(Entry &entry, int newZOrder) noexcept;
    auto removeItem(Entry &entry) noexcept -> bool;

    [[nodiscard]] auto begin() noexcept -> Entry;
    [[nodiscard]] auto end() noexcept -> Entry;

    [[nodiscard]] auto begin() const noexcept -> Entry;
    [[nodiscard]] auto end() const noexcept -> Entry;

  private:
    [[nodiscard]] auto generateNextId(int zOrder) noexcept -> std::size_t;
  };

  /////////////////////////////////////////////////////////////////////////////
  class SfItemStore::Entry
  {
    friend class SfItemStore;

    std::map<std::size_t, SfBoardItem>::iterator itemIter_;
    std::map<std::size_t, SfBoardItem> *itemMapPtr_;
    bool isMutable_;

    Entry(
        std::map<std::size_t, SfBoardItem>::iterator itemIter,
        std::map<std::size_t, SfBoardItem> *itemMapPtr,
        bool isMutable = true) noexcept;

  public:
    /**
     * Construct an null Entry object associated with an ItemStore
     *
     */
    Entry(SfItemStore const &itemStore) noexcept;

    void operator++() noexcept;
    [[nodiscard]] auto operator*() noexcept -> SfBoardItem &;
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
