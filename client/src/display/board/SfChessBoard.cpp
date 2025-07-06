// SfChessBoard.cpp

#include <boost/assert.hpp>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "SfChessBoard.h"
#include "SfBoardState.h"

namespace bgg
{

  SfChessBoard::SfChessBoard(sf::Vector2i const &currentWndSize,
                             sf::Vector2i paddingTopLeft,
                             sf::Vector2i paddingBottomRight,
                             std::shared_ptr<SfGameRuleAdapter> gameRule,
                             SfItemStore itemStore,
                             SfTileMap tileMap) noexcept
      : paddingTopLeft_(std::move(paddingTopLeft)),
        paddingBottomRight_(std::move(paddingBottomRight)),
        gameRule_(gameRule),
        itemStore_(std::move(itemStore)),
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
    else if (auto wndResized = event.getIf<sf::Event::Resized>())
    {
      // fitWindow(sf::Vector2i(wndResized->size));
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
    for (SfBoardItem &item : itemStore_)
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

    for (SfBoardItem &item : itemStore_)
    {
      item.move(offset);
      item.scale(sf::Vector2f{scaleFactor, scaleFactor});
    }
  }
} // namespace bgg
