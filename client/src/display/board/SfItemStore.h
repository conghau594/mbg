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
  class SfItemStore
  {
    std::shared_ptr<const SfTextureAtlas> const itemTextureAtlas_;

    mutable std::map<std::size_t, SfBoardItem> boardItems_;
    std::atomic<std::size_t> nextBaseItemId_; ///< Id generator for the boardItems_ (std::map )

    constexpr static int Z_ORDER_BIT_COUNT = 8;
    constexpr static int MAX_Z_ORDER = 1 << Z_ORDER_BIT_COUNT;

  public:
    class Entry;

    SfItemStore(SfItemStore &&other) noexcept;
    SfItemStore(std::shared_ptr<const SfTextureAtlas> itemTextureAtlas) noexcept;

    // auto addItem(SfBoardItem item, int zOrder, bool visible) noexcept
    //     -> std::optional<Entry>;

    auto addItem(
        int textureCellIndex,
        int zOrder,
        std::string name = "",
        bool visible = true) noexcept -> Entry;

    auto removeItem(std::size_t itemId) noexcept -> bool;

    auto removeItem(Entry &entry) noexcept -> bool;

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

    Entry(
        std::map<std::size_t, SfBoardItem>::iterator itemIter,
        std::map<std::size_t, SfBoardItem> *itemMapPtr) noexcept;

  public:
    void operator++() noexcept;
    [[nodiscard]] auto operator*() noexcept -> SfBoardItem &;
    [[nodiscard]] auto operator==(Entry const &rhs) const noexcept -> bool;

    /**
     * An Entry object is null when and only when it's removed from its SfItemStore.
     * We cannot construct an null Entry object.
     */
    [[nodiscard]] auto isNull() const noexcept -> bool;

    /**
     * An assertion raises if the Entry object is null
     *
     */
    [[nodiscard]] auto getId() const noexcept -> std::size_t const &;

    /**
     * An assertion raises if the Entry object is null
     *
     */
    [[nodiscard]] auto getItem() noexcept -> SfBoardItem &;
  };
} // namespace bgg
