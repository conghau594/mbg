// SfItemStore.cpp

#include <boost/assert.hpp>

#include "SfItemStore.h"

namespace bgg
{
  SfItemStore::SfItemStore(
      std::shared_ptr<const SfTextureAtlas> itemTextureAtlas) noexcept
      : itemTextureAtlas_(std::move(itemTextureAtlas)),
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
      bool visible) noexcept -> SfItemStore::Entry
  {
    sf::IntRect textureCellRect = itemTextureAtlas_->getRegion(textureCellIndex);
    size_t id = generateNextId(zOrder);

    auto [iter, inserted] = boardItems_.try_emplace(
        id, itemTextureAtlas_, textureCellRect, std::move(name), visible);

    BOOST_ASSERT_MSG(inserted, "With a unique id, emplacing to boardItems_ should not fail");

    return Entry(iter, &boardItems_);
  }

  // auto SfItemStore::removeItem(std::size_t itemId) noexcept -> bool
  // {
  //   return bool(boardItems_.erase(itemId));
  // }

  auto SfItemStore::getZOrder(Entry const &entry) const noexcept -> int
  {
    constexpr int NUM_SHIFTED_BITS = 8 * sizeof(std::size_t) - Z_ORDER_BIT_COUNT;
    return entry.itemIter_->first >> NUM_SHIFTED_BITS;
  }

  void SfItemStore::changeZOrder(Entry &entry, int newZOrder) noexcept
  {
    BOOST_VERIFY_MSG(
        !entry.isNull() && entry.itemMapPtr_ == &boardItems_,
        "This entry should not be null and its store must be this store");

    std::size_t newId = generateNextId(newZOrder);

    auto [iter, inserted] = boardItems_.try_emplace(
        newId, std::move(entry.getItem()));

    BOOST_ASSERT_MSG(
        inserted,
        "With a unique id, emplacing to boardItems_ should not fail");

    removeItem(entry);
    entry = Entry(iter, &boardItems_);
  }

  auto SfItemStore::removeItem(Entry &entry) noexcept -> bool
  {
    if (entry.isNull() || entry.itemMapPtr_ != &boardItems_)
    {
      return false;
    }

    boardItems_.erase(entry.itemIter_);
    entry.itemMapPtr_ = nullptr; // make sure the entry is null
    entry.itemIter_ = boardItems_.end();
    return true;
  }

  auto SfItemStore::begin() noexcept -> SfItemStore::Entry
  {
    return Entry(boardItems_.begin(), &boardItems_);
  }

  auto SfItemStore::end() noexcept -> SfItemStore::Entry
  {
    return Entry(boardItems_.end(), &boardItems_);
  }

  auto SfItemStore::begin() const noexcept -> SfItemStore::Entry
  {
    return Entry(boardItems_.begin(), &boardItems_, false);
  }

  auto SfItemStore::end() const noexcept -> SfItemStore::Entry
  {
    return Entry(boardItems_.end(), &boardItems_, false);
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
  SfItemStore::Entry::Entry(
      std::map<std::size_t, SfBoardItem>::iterator itemIter,
      std::map<std::size_t, SfBoardItem> *itemMapPtr,
      bool isMutable) noexcept
      : itemIter_(std::move(itemIter)),
        itemMapPtr_(itemMapPtr),
        isMutable_(isMutable)
  {
  }

  SfItemStore::Entry::Entry(SfItemStore const &itemStore) noexcept
      : Entry(itemStore.end())
  {
  }

  void SfItemStore::Entry::operator++() noexcept
  {
    ++itemIter_;
  }

  auto SfItemStore::Entry::operator*() noexcept -> SfBoardItem &
  {
    return getItem();
  }

  auto SfItemStore::Entry::operator==(
      SfItemStore::Entry const &rhs) const noexcept -> bool
  {
    return (itemMapPtr_ == rhs.itemMapPtr_) && (itemIter_ == rhs.itemIter_);
  }

  auto SfItemStore::Entry::isNull() const noexcept -> bool
  {
    return (itemMapPtr_ == nullptr) || (itemIter_ == itemMapPtr_->end());
  }

  // auto SfItemStore::Entry::getId() const noexcept -> std::size_t const &
  // {
  //   BOOST_ASSERT_MSG(!isNull(), "Invalid entry. It may be removed from its map.");
  //   return itemIter_->first;
  // }

  auto SfItemStore::Entry::getItem() noexcept -> SfBoardItem &
  {
    BOOST_ASSERT_MSG(!isNull(), "Invalid entry. It may be removed from its map.");
    BOOST_ASSERT_MSG(isMutable_, "This entry is immutable because it was produced"
                                 " from a const ItemStore");

    return itemIter_->second;
  }
} // namespace bgg
