// SfChessBoard.cpp

#include <list>

#include <boost/assert.hpp>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "SfChessBoard.h"
#include "SfBoardState.h"
#include "SfGameRuleAdapter.h"

namespace bgg
{
  SfChessBoard::SfChessBoard(
      sf::IntRect const &boardRect,
      std::shared_ptr<SfGameRuleAdapter> gameRule,
      std::shared_ptr<SfTileMap> tileMap,
      std::shared_ptr<SfItemStore> itemStore) noexcept
      : gameRule_(std::move(gameRule)),
        tileMap_(std::move(tileMap)),
        itemStore_(std::move(itemStore))
  {
    fitRectangle(boardRect);

    SfItemPlacementMap itemPlacements = gameRule_->getItemPlacements();
    for (auto &[tile, entry] : itemPlacements)
    {
      tileMap_->fitItemToTile(entry.getItem(), tile);
    }
  }

  void SfChessBoard::onEvent(sf::Event const &event) noexcept
  {
    BOOST_ASSERT_MSG(currentBoardState_,
                     "currentBoardState_ of SfChessBoard cannot be null");
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
        currentBoardState_->onMouseReleased(mouseBtnReleased->position);
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
    target.draw(*tileMap_, states);
    for (SfBoardItem &item : *itemStore_)
    {
      if (item.isVisible())
      {
        target.draw(item, states);
      }
    }
  }

  void SfChessBoard::changeState(
      std::shared_ptr<SfBoardState> newState) noexcept
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

  void SfChessBoard::fitRectangle(sf::IntRect const &boardRect) noexcept
  {
    sf::Vector2i const &maxBoardSize = boardRect.size;
    sf::Vector2i mapSize = tileMap_->getSize();
    sf::Vector2i mapPosition = tileMap_->getPosition();

    float scaleFactor;
    sf::Vector2i moveVector;

    if (maxBoardSize.x * mapSize.y > maxBoardSize.y * mapSize.x)
    {
      scaleFactor = float(maxBoardSize.y) / float(mapSize.y);
      int newBoardSideWidth = int(scaleFactor * float(mapSize.x));

      sf::Vector2i newPosition{
          boardRect.position.x + (maxBoardSize.x - newBoardSideWidth) / 2,
          boardRect.position.y};
      moveVector = newPosition - mapPosition;
    }
    else
    {
      scaleFactor = float(maxBoardSize.x) / float(mapSize.x);
      int newBoardSideHeight = int(scaleFactor * float(mapSize.y));

      sf::Vector2i newPosition{
          boardRect.position.x,
          boardRect.position.y + (maxBoardSize.y - newBoardSideHeight) / 2};
      moveVector = newPosition - mapPosition;
    }

    tileMap_->move(moveVector);
    tileMap_->scale(sf::Vector2f{scaleFactor, scaleFactor});
  }
} // namespace bgg
