// SfItemStore.cpp

#include <boost/assert.hpp>

#include "SfItemStore.h"

namespace bgg
{
  std::atomic<size_t> SfItemStore::nextMapId{1};

  SfItemStore::SfItemStore(SfItemStore &&other) noexcept
      : itemTextureAtlas_(std::move(other.itemTextureAtlas_)),
        boardItems_(std::move(other.boardItems_)),
        id_(other.id_),
        nextBaseItemId_(other.nextBaseItemId_.load())
  {
    other.id_ = 0;
  }

  SfItemStore::SfItemStore(
      std::shared_ptr<const SfTextureAtlas> itemTextureAtlas) noexcept
      : itemTextureAtlas_(std::move(itemTextureAtlas)),
        id_(nextMapId++),
        nextBaseItemId_(0)
  {
  }

  // auto SfItemStore::addItem(SfBoardItem item, int zOrder, bool visible) noexcept
  //     -> std::optional<SfItemStore::Entry>
  // {
  //   size_t id = generateNextId(zOrder);
  //   auto [iter, inserted] = boardItems_.try_emplace(id, std::move(item));
  //   iter->second.setVisible(visible);
  //   return inserted ? Entry(iter, id_) : std::optional<Entry>(std::nullopt);
  // }

  auto SfItemStore::addItem(
      int textureCellIndex,
      int zOrder,
      std::string name,
      bool visible) noexcept
      -> std::optional<SfItemStore::Entry>
  {
    sf::IntRect textureCellRect = itemTextureAtlas_->getRegion(textureCellIndex);
    size_t id = generateNextId(zOrder);

    auto [iter, inserted] = boardItems_.try_emplace(
        id, itemTextureAtlas_, textureCellRect, std::move(name), visible);
    return inserted ? Entry(iter, id_) : std::optional<Entry>(std::nullopt);
  }

  auto SfItemStore::removeItem(std::size_t itemId) noexcept -> bool
  {
    return bool(boardItems_.erase(itemId));
  }

  auto SfItemStore::removeItem(Entry &entry) noexcept -> bool
  {
    if (entry.isNull() || entry.mapId_ == id_ || entry.iter_ != boardItems_.end())
    {
      return false;
    }

    boardItems_.erase(entry.iter_);
    entry.mapId_ = 0; // make sure the entry is null
    entry.iter_ = boardItems_.end();
    return true;
  }

  auto SfItemStore::begin() const noexcept -> SfItemStore::Entry
  {
    return Entry(boardItems_.begin(), id_);
  }

  auto SfItemStore::end() const noexcept -> SfItemStore::Entry
  {
    return Entry(boardItems_.end(), id_);
  }

  auto SfItemStore::generateNextId(int zOrder) noexcept -> std::size_t
  {
    if (zOrder < 0)
    {
      zOrder = 0;
    }
    else if (zOrder > MAX_Z_ORDER)
    {
      zOrder = MAX_Z_ORDER;
    }

    constexpr int NUM_SHIFTED_BITS = 8 * sizeof(std::size_t) - Z_ORDER_BIT_COUNT;
    std::size_t returnedId = nextBaseItemId_++;
    BOOST_ASSERT_MSG(returnedId < (1ull << NUM_SHIFTED_BITS),
                     "This failure may never occur");

    return (std::size_t(zOrder) << NUM_SHIFTED_BITS) | returnedId;
  }

  /////////////////////////////////////////////////////////////////////////////
  SfItemStore::Entry::Entry(std::map<std::size_t, SfBoardItem>::iterator iter, std::size_t mapId)
      : iter_(std::move(iter)), mapId_(mapId)
  {
  }

  void SfItemStore::Entry::operator++() noexcept
  {
    ++iter_;
  }

  auto SfItemStore::Entry::operator*() noexcept -> SfBoardItem &
  {
    return getItem();
  }

  auto SfItemStore::Entry::operator==(SfItemStore::Entry const &rhs) const noexcept -> bool
  {
    return (iter_ == rhs.iter_) && (mapId_ == rhs.mapId_);
  }

  auto SfItemStore::Entry::isNull() const noexcept -> bool
  {
    return mapId_ == 0;
  }

  auto SfItemStore::Entry::getId() const noexcept -> std::size_t const &
  {
    BOOST_ASSERT_MSG(!isNull(), "Invalid entry. It may be removed from its map.");
    return iter_->first;
  }

  auto SfItemStore::Entry::getItem() noexcept -> SfBoardItem &
  {
    BOOST_ASSERT_MSG(!isNull(), "Invalid entry. It may be removed from its map.");
    return iter_->second;
  }
} // namespace bgg
