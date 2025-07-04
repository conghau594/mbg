// SfChessBoard.cpp

#include <boost/assert.hpp>

#include <SFML/Window/Event.hpp>

#include "SfChessBoard.h"
#include "SfBoardState.h"

namespace bgg
{
  SfChessBoard::SfChessBoard(SfTileMap tileMap) noexcept
      : currentBoardState_(nullptr),
        lastBoardState_(nullptr),
        tileMap_(std::move(tileMap)),
        nextBaseId_{0}
  {
  }

  void SfChessBoard::onEvent(sf::Event const &event) noexcept
  {
    BOOST_ASSERT_MSG(currentBoardState_, "currentBoardState_ of SfChessBoard cannot be null");
    if (lastBoardState_ != currentBoardState_)
    {
      lastBoardState_ = currentBoardState_;
    }

    if (auto mouseMoved = event.getIf<sf::Event::MouseMoved>())
    {
      currentBoardState_->onMouseMoved(mouseMoved->position);
    }
    else if (auto mouseBtnPressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
      if (mouseBtnPressed->button == sf::Mouse::Button::Left)
      {
        currentBoardState_->onMousePressed(mouseBtnPressed->position);
      }
    }
    else if (auto mouseBtnReleased = event.getIf<sf::Event::MouseButtonReleased>())
    {
      if (mouseBtnReleased->button == sf::Mouse::Button::Left)
      {
        currentBoardState_->onMousePressed(mouseBtnReleased->position);
      }
    }
  }

  void SfChessBoard::send(ClientEvent const &request) noexcept
  {
  }

  void SfChessBoard::draw(sf::RenderTarget &target, sf::RenderStates states) const noexcept
  {
    // draw the tileMap
    target.draw(tileMap_, states);
    for (auto &[id, item] : boardItems_)
    {
      target.draw(item, states);
    }
  }

  void SfChessBoard::changeState(std::shared_ptr<SfBoardState> newState) noexcept
  {
    if (currentBoardState_ != nullptr)
    {
      currentBoardState_->onExit();
    }

    currentBoardState_ = newState;
    if (newState != nullptr)
    {
      newState->onEnter();
    }
  }

  auto SfChessBoard::addItem(SfBoardItem item, int zOrder, bool visible) noexcept
      -> std::pair<std::size_t, SfBoardItem *>
  {
    size_t id = generateNextId(zOrder);
    auto [iter, inserted] = boardItems_.try_emplace(id, std::move(item));
    iter->second.setVisible(visible);
    SfBoardItem *returnedPtr = inserted ? &(iter->second) : nullptr;
    return {id, returnedPtr};
  }

  auto SfChessBoard::addItem(
      sf::Vector2i tileCoords, int textureCellIndex, int zOrder, bool visible) noexcept
      -> std::pair<std::size_t, SfBoardItem *>
  {
    size_t id = generateNextId(zOrder);
    // auto [iter, inserted] = boardItems_.try_emplace(id, std::move(item));
    // iter->second.setVisible(visible);
    // SfBoardItem item = inserted ? &(iter->second) : nullptr;
    return {id, nullptr};
  }

  auto SfChessBoard::removeItem(std::size_t itemId) noexcept -> bool
  {
    return bool(boardItems_.erase(itemId));
  }

  auto SfChessBoard::putItemOnTile(SfBoardItem *item, sf::Vector2i tileCoords) noexcept -> bool
  {
    return false;
  }

  auto SfChessBoard::generateNextId(int zOrder) noexcept -> size_t
  {
    if (zOrder < 1)
    {
      zOrder = 1;
    }
    else if (zOrder > MAX_Z_ORDER)
    {
      zOrder = MAX_Z_ORDER;
    }

    constexpr int NUM_SHIFTED_BITS = 8 * sizeof(std::size_t) - Z_ORDER_BIT_COUNT;
    size_t returnedId = nextBaseId_++;
    BOOST_ASSERT_MSG(returnedId < (1ull << NUM_SHIFTED_BITS), "This failure may never happens");

    return (size_t(zOrder) << NUM_SHIFTED_BITS) | returnedId;
  }
} // namespace bgg
