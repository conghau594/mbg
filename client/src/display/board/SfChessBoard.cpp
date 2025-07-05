// SfChessBoard.cpp

#include <boost/assert.hpp>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "SfChessBoard.h"
#include "SfBoardState.h"

#ifdef _DEBUG
#include <iostream>
#include <format>
#endif

namespace bgg
{

  SfChessBoard::SfChessBoard(sf::Vector2i const &currentWndSize,
                             sf::Vector2i paddingTopLeft,
                             sf::Vector2i paddingBottomRight,
                             std::shared_ptr<const SfTextureAtlas> itemTextureAtlas,
                             SfTileMap tileMap) noexcept
      : paddingTopLeft_(std::move(paddingTopLeft)),
        paddingBottomRight_(std::move(paddingBottomRight)),
        itemTextureAtlas_(std::move(itemTextureAtlas)),
        tileMap_(std::move(tileMap))
  {
    fitWindow(currentWndSize);
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
    // TODO: SfChessBoard::send(ClientEvent const &request) noexcept
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
      -> std::pair<const std::size_t, SfBoardItem> *
  {
    size_t id = generateNextId(zOrder);
    auto [iter, inserted] = boardItems_.try_emplace(id, std::move(item));
    iter->second.setVisible(visible);
    return inserted ? &(*iter) : nullptr;
  }

  auto SfChessBoard::addItem(
      sf::Vector2i const &tileCoords,
      int textureCellIndex,
      int zOrder,
      std::string name,
      bool visible) noexcept
      -> std::pair<const std::size_t, SfBoardItem> *
  {
    sf::IntRect textureCellRect = itemTextureAtlas_->getRegion(textureCellIndex);
    size_t id = generateNextId(zOrder);

    auto [iter, inserted] = boardItems_.try_emplace(
        id, itemTextureAtlas_, textureCellRect, std::move(name), visible);
    std::pair<const std::size_t, SfBoardItem> *itemEntry = inserted ? &(*iter) : nullptr;

    if (itemEntry != nullptr)
    {
      SfBoardItem &item = itemEntry->second;
      sf::IntRect itemRect = tileMap_.tileToScreenRect(tileCoords);
      sf::Vector2f tileSize = sf::Vector2f(itemRect.size);
      sf::Vector2f scaleFactors = tileSize.componentWiseDiv(sf::Vector2f(textureCellRect.size));

      item.setScale(scaleFactors);
      item.setPosition(itemRect.position);

#ifdef _DEBUG
      std::string debugInfo = std::format(
          "\nAdd texture of '{}' (cell {}) at pos ({}, {})"
          " with size ({}, {}) and origin ({}, {})",
          item.getName(),
          textureCellIndex,
          item.getPosition().x, item.getPosition().y,
          item.getSize().x, item.getSize().y,
          item.getOrigin().x, item.getOrigin().y);

      std::clog << debugInfo;
#endif
    }

    return itemEntry;
  }

  auto SfChessBoard::removeItem(std::size_t itemId) noexcept -> bool
  {
    return bool(boardItems_.erase(itemId));
  }

  auto SfChessBoard::putItemOnTile(
      SfBoardItem *item, sf::Vector2i const &tileCoords) noexcept -> bool
  {
    // TODO:
    if (item == nullptr)
    {
      return false;
    }

    sf::IntRect itemRect = tileMap_.tileToScreenRect(tileCoords);
    item->setPosition(itemRect.position);

    return true;
  }

  auto SfChessBoard::generateNextId(int zOrder) noexcept -> size_t
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
    size_t returnedId = nextBaseItemId_++;
    BOOST_ASSERT_MSG(returnedId < (1ull << NUM_SHIFTED_BITS),
                     "This failure may never occur");

    return (size_t(zOrder) << NUM_SHIFTED_BITS) | returnedId;
  }

  void SfChessBoard::fitWindow(sf::Vector2i const &wndSize) noexcept
  {
    sf::Vector2i maxBoardSize = wndSize - paddingTopLeft_ - paddingBottomRight_;
    sf::Vector2i mapSize = tileMap_.getSize();
    sf::Vector2i mapPosition = tileMap_.getPosition();

    float scaleFactor;
    sf::Vector2i offset;

    if (maxBoardSize.x * mapSize.y > maxBoardSize.y * mapSize.x)
    {
      scaleFactor = float(maxBoardSize.y) / float(mapSize.y);
      int newBoardSideWidth = int(scaleFactor * mapSize.x);

      sf::Vector2i newPosition{
          paddingTopLeft_.x + (maxBoardSize.x - newBoardSideWidth) / 2,
          paddingTopLeft_.y};
      offset = newPosition - mapPosition;
    }
    else
    {
      scaleFactor = float(maxBoardSize.x) / float(mapSize.x);
      int newBoardSideHeight = int(scaleFactor * mapSize.y);

      sf::Vector2i newPosition{
          paddingTopLeft_.x,
          paddingTopLeft_.y + (maxBoardSize.y - newBoardSideHeight) / 2};
      offset = newPosition - mapPosition;
    }

    tileMap_.move(offset);
    tileMap_.scale(sf::Vector2f{scaleFactor, scaleFactor});

    for (auto &[id, item] : boardItems_)
    {
      item.move(offset);
      item.scale(sf::Vector2f{scaleFactor, scaleFactor});
    }
  }
} // namespace bgg
