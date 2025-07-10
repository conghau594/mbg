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

    return Entry(
        std::make_shared<Container::iterator>(iter),
        &boardItems_);
  }

  auto SfItemStore::getZOrder(Entry const &entry) const noexcept -> int
  {
    constexpr int NUM_SHIFTED_BITS = 8 * sizeof(std::size_t) - Z_ORDER_BIT_COUNT;
    return int((*entry.itemIter_)->first >> NUM_SHIFTED_BITS);
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

    boardItems_.erase(*(entry.itemIter_));
    *(entry.itemIter_) = iter;
  }

  auto SfItemStore::removeItem(Entry &entry) noexcept -> bool
  {
    if (entry.isNull() || (entry.itemMapPtr_ != &boardItems_))
    {
      return false;
    }

    boardItems_.erase(*(entry.itemIter_));
    // entry.itemMapPtr_ = nullptr;
    entry.itemIter_ = nullptr; // make sure the entry is null but it still belongs to this store
    return true;
  }

  auto SfItemStore::begin() noexcept -> SfItemStore::Iter
  {
    return Iter(boardItems_.begin(), &boardItems_);
  }

  auto SfItemStore::end() noexcept -> SfItemStore::Iter
  {
    return Iter(boardItems_.end(), &boardItems_);
  }

  auto SfItemStore::begin() const noexcept -> SfItemStore::Iter
  {
    return Iter(boardItems_.cbegin(), &boardItems_);
  }

  auto SfItemStore::end() const noexcept -> SfItemStore::Iter
  {
    return Iter(boardItems_.cend(), &boardItems_);
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
    std::size_t baseId = nextBaseItemId_++;
    BOOST_ASSERT_MSG(baseId < (1ull << NUM_SHIFTED_BITS),
                     "This failure may never occur");

    return (std::size_t(zOrder) << NUM_SHIFTED_BITS) | baseId;
  }

  /////////////////////////////////////////////////////////////////////////////
  SfItemStore::Iter::Iter(
      std::variant<Container::iterator, Container::const_iterator> itemIter,
      Container const *const itemMapPtr) noexcept
      : itemIter_(std::move(itemIter)),
        itemMapPtr_(itemMapPtr)
  {
  }

  SfItemStore::Iter::Iter(SfItemStore const &itemStore) noexcept
      : Iter(itemStore.end())
  {
  }

  void SfItemStore::Iter::operator++() noexcept
  {
    ++(std::get<Container::iterator>(itemIter_));
  }

  void SfItemStore::Iter::operator++() const noexcept
  {
    ++(std::get<Container::const_iterator>(itemIter_));
  }

  auto SfItemStore::Iter::operator*() noexcept -> SfBoardItem &
  {
    return std::get<Container::iterator>(itemIter_)->second;
  }

  auto SfItemStore::Iter::operator*() const noexcept -> SfBoardItem const &
  {
    return std::get<Container::const_iterator>(itemIter_)->second;
  }

  auto SfItemStore::Iter::operator==(
      SfItemStore::Iter const &rhs) const noexcept -> bool
  {
    return (itemMapPtr_ == rhs.itemMapPtr_) && (itemIter_ == rhs.itemIter_);
  }

  /////////////////////////////////////////////////////////////////////////////
  SfItemStore::Entry::Entry(
      std::shared_ptr<Container::iterator> itemIter,
      Container const *itemMapPtr) noexcept
      : itemIter_(std::move(itemIter)),
        itemMapPtr_(itemMapPtr)
  {
  }

  SfItemStore::Entry::Entry(SfItemStore const &itemStore) noexcept
      : Entry(nullptr, &itemStore.boardItems_)
  {
  }

  auto SfItemStore::Entry::operator==(
      SfItemStore::Entry const &rhs) const noexcept -> bool
  {
    return (itemMapPtr_ == rhs.itemMapPtr_) && (itemIter_ == rhs.itemIter_);
  }

  auto SfItemStore::Entry::isNull() const noexcept -> bool
  {
    return (itemMapPtr_ == nullptr) || (itemIter_ == nullptr);
  }

  // auto SfItemStore::Entry::getId() const noexcept -> std::size_t const &
  // {
  //   BOOST_ASSERT_MSG(!isNull(), "Invalid entry. It may be removed from its map.");
  //   return itemIter_->first;
  // }

  auto SfItemStore::Entry::getItem() noexcept -> SfBoardItem &
  {
    BOOST_ASSERT_MSG(!isNull(), "Invalid entry. It may be removed from its map.");

    return (*itemIter_)->second;
  }
} // namespace bgg
