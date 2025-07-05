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

  SfChessBoard::SfChessBoard(std::shared_ptr<sf::RenderWindow> window,
                             sf::Vector2i boundaryTopLeft,
                             SfTextureAtlas atlas,
                             sf::Vector2i mapSizeInTiles,
                             std::vector<sf::Vector2i> const &tileLayout) noexcept
      : window_(std::move(window)),
        boundaryTopLeft_(std::move(boundaryTopLeft)),
        atlas_(std::move(atlas)),
        tileMap_(&atlas_, std::move(mapSizeInTiles), tileLayout)
  {
    fitWindow();
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
      -> std::pair<std::size_t, SfBoardItem *>
  {
    // TODO:
    size_t id = generateNextId(zOrder);
    auto [iter, inserted] = boardItems_.try_emplace(id, std::move(item));
    iter->second.setVisible(visible);
    SfBoardItem *itemPtr = inserted ? &(iter->second) : nullptr;
    return {id, itemPtr};
  }

  auto SfChessBoard::addItem(
      sf::Vector2i const &tileCoords,
      sf::Vector2i const &textureCellIndex,
      int zOrder,
      std::string name,
      bool visible) noexcept -> std::pair<std::size_t, SfBoardItem *>
  {
    // TODO:
    sf::Vector2i textureCellTopLeft = atlas_.cellSize.componentWiseMul(textureCellIndex);
    sf::IntRect textureCellRect(textureCellTopLeft, atlas_.cellSize);
    size_t id = generateNextId(zOrder);

    auto [iter, inserted] = boardItems_.try_emplace(
        id, atlas_.texture, textureCellRect, std::move(name), visible);
    SfBoardItem *itemPtr = inserted ? &(iter->second) : nullptr;
    if (itemPtr != nullptr)
    {
      sf::IntRect itemRect = tileToScreenRect(tileCoords);
      sf::Vector2f tileSize = sf::Vector2f(itemRect.size);
      sf::Vector2f scaleFactors = tileSize.componentWiseDiv(sf::Vector2f(atlas_.cellSize));

      itemPtr->setScale(scaleFactors);
      itemPtr->setPosition(sf::Vector2f(itemRect.position));

#ifdef _DEBUG
      std::string debugInfo = std::format(
          "\nAdd texture of '{}' ({}, {}) at pos ({}, {})"
          " with size ({}, {}) and origin ({}, {})",
          itemPtr->getName(),
          textureCellIndex.x, textureCellIndex.y,
          itemPtr->getPosition().x, itemPtr->getPosition().y,
          itemPtr->getSize().x, itemPtr->getSize().y,
          itemPtr->getOrigin().x, itemPtr->getOrigin().y);

      std::clog << debugInfo;
#endif
    }

    return {id, itemPtr};
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

    sf::IntRect itemRect = tileToScreenRect(tileCoords);
    item->setPosition(sf::Vector2f(itemRect.position));

    // sf::Vector2f tileSize = sf::Vector2f(itemRect.size);
    //  sf::Vector2f scaleFactors = tileSize.componentWiseDiv(sf::Vector2f(atlas_.cellSize));
    //  item->setScale(scaleFactors);

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
                     "This failure may never happens");

    return (size_t(zOrder) << NUM_SHIFTED_BITS) | returnedId;
  }

  auto SfChessBoard::screenToTile(sf::Vector2i const &screenCoords) noexcept -> sf::Vector2i
  {
    return (screenCoords - boardTopLeft_).componentWiseDiv(tileSizeOnScreen_);
  }

  auto SfChessBoard::tileToScreenRect(sf::Vector2i const &tileCoords) noexcept -> sf::IntRect
  {
    // TODO:
    sf::Vector2i rectTopLeft = boardTopLeft_ + tileSizeOnScreen_.componentWiseMul(tileCoords);
    return sf::IntRect(rectTopLeft, tileSizeOnScreen_);
  }

  void SfChessBoard::fitWindow() noexcept
  {
    sf::Vector2i maxBoardSize = sf::Vector2i(window_->getSize()) - boundaryTopLeft_;
    int boardSideLength;

    if (maxBoardSize.x > maxBoardSize.y)
    {
      boardSideLength = maxBoardSize.y;
      boardTopLeft_.x = (maxBoardSize.x - boardSideLength) / 2 + boundaryTopLeft_.x;
      boardTopLeft_.y = boundaryTopLeft_.y;
    }
    else
    {
      boardSideLength = maxBoardSize.x;
      boardTopLeft_.x = boundaryTopLeft_.x;
      boardTopLeft_.y = (maxBoardSize.y - boardSideLength) / 2 + boundaryTopLeft_.y;
    }

    tileSizeOnScreen_ = sf::Vector2i(boardSideLength / 8, boardSideLength / 8);
    sf::Vector2f scaleFactors = sf::Vector2f(tileSizeOnScreen_)
                                    .componentWiseDiv(sf::Vector2f(atlas_.cellSize));

    tileMap_.setScale(scaleFactors);
    tileMap_.setPosition(sf::Vector2f(boardTopLeft_));
  }
} // namespace bgg
