// ItemStore.cpp

#include <boost/assert.hpp>

#include "ItemStore.h"

namespace bgg
{
  ItemStore::ItemStore(
      std::shared_ptr<const BaseTextureAtlas> itemTextureAtlas) noexcept
      : boardItems_(),
        itemTextureAtlas_(std::move(itemTextureAtlas)),
        nextBaseItemId_(0)
  {
  }

  auto ItemStore::addItem(
      int textureCellIndex,
      int zOrder,
      std::string name,
      bool visible) noexcept -> ItemStore::Entry
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

  auto ItemStore::getZOrder(Entry const &entry) const noexcept -> int
  {
    constexpr int NUM_SHIFTED_BITS = 8 * sizeof(std::size_t) - Z_ORDER_BIT_COUNT;
    return int((*entry.itemIter_)->first >> NUM_SHIFTED_BITS);
  }

  void ItemStore::changeZOrder(Entry &entry, int newZOrder) noexcept
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

  auto ItemStore::removeItem(Entry &entry) noexcept -> bool
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

  auto ItemStore::begin() noexcept -> ItemStore::Iter
  {
    return Iter(boardItems_.begin(), &boardItems_);
  }

  auto ItemStore::end() noexcept -> ItemStore::Iter
  {
    return Iter(boardItems_.end(), &boardItems_);
  }

  auto ItemStore::begin() const noexcept -> ItemStore::Iter
  {
    return Iter(boardItems_.cbegin(), &boardItems_);
  }

  auto ItemStore::end() const noexcept -> ItemStore::Iter
  {
    return Iter(boardItems_.cend(), &boardItems_);
  }

  auto ItemStore::generateNextId(int zOrder) noexcept -> std::size_t
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
  ItemStore::Iter::Iter(
      std::variant<Container::iterator, Container::const_iterator> itemIter,
      Container const *const itemMapPtr) noexcept
      : itemIter_(std::move(itemIter)),
        itemMapPtr_(itemMapPtr)
  {
  }

  ItemStore::Iter::Iter(ItemStore const &itemStore) noexcept
      : Iter(itemStore.end())
  {
  }

  void ItemStore::Iter::operator++() noexcept
  {
    ++(std::get<Container::iterator>(itemIter_));
  }

  void ItemStore::Iter::operator++() const noexcept
  {
    ++(std::get<Container::const_iterator>(itemIter_));
  }

  auto ItemStore::Iter::operator*() noexcept -> BoardItem &
  {
    return std::get<Container::iterator>(itemIter_)->second;
  }

  auto ItemStore::Iter::operator*() const noexcept -> BoardItem const &
  {
    return std::get<Container::const_iterator>(itemIter_)->second;
  }

  auto ItemStore::Iter::operator==(
      ItemStore::Iter const &rhs) const noexcept -> bool
  {
    return (itemMapPtr_ == rhs.itemMapPtr_) && (itemIter_ == rhs.itemIter_);
  }

  /////////////////////////////////////////////////////////////////////////////
  ItemStore::Entry::Entry(
      std::shared_ptr<Container::iterator> itemIter,
      Container const *itemMapPtr) noexcept
      : itemIter_(std::move(itemIter)),
        itemMapPtr_(itemMapPtr)
  {
  }

  ItemStore::Entry::Entry(ItemStore const &itemStore) noexcept
      : Entry(nullptr, &itemStore.boardItems_)
  {
  }

  auto ItemStore::Entry::operator==(
      ItemStore::Entry const &rhs) const noexcept -> bool
  {
    return (itemMapPtr_ == rhs.itemMapPtr_) && (itemIter_ == rhs.itemIter_);
  }

  auto ItemStore::Entry::isNull() const noexcept -> bool
  {
    return (itemMapPtr_ == nullptr) || (itemIter_ == nullptr);
  }

  // auto ItemStore::Entry::getId() const noexcept -> std::size_t const &
  // {
  //   BOOST_ASSERT_MSG(!isNull(), "Invalid entry. It may be removed from its map.");
  //   return itemIter_->first;
  // }

  auto ItemStore::Entry::getItem() noexcept -> BoardItem &
  {
    BOOST_ASSERT_MSG(!isNull(), "Invalid entry. It may be removed from its map.");

    return (*itemIter_)->second;
  }
} // namespace bgg
