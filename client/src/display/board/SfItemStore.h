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
    std::size_t id_;                          ///< A map with id_ == 0 is an invalid map
    std::atomic<std::size_t> nextBaseItemId_; ///< Id generator for the boardItems_ (std::map )

    static std::atomic<size_t> nextMapId;
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
        bool visible = true) noexcept
        -> std::optional<Entry>;

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

    std::map<std::size_t, SfBoardItem>::iterator iter_;
    std::size_t mapId_{0};

    Entry(std::map<std::size_t, SfBoardItem>::iterator iter, std::size_t mapId);

  public:
    void operator++() noexcept;
    [[nodiscard]] auto operator*() noexcept -> SfBoardItem &;
    [[nodiscard]] auto operator==(Entry const &rhs) const noexcept -> bool;

    [[nodiscard]] auto isNull() const noexcept -> bool;
    [[nodiscard]] auto getId() const noexcept -> std::size_t const &;
    [[nodiscard]] auto getItem() noexcept -> SfBoardItem &;
  };
} // namespace bgg
