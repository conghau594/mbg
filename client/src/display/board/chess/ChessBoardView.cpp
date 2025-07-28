// ChessBoardView.cpp

#include <list>

#include <boost/assert.hpp>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "ChessBoardView.h"
#include "display/board/IBoardViewState.h"
#include "IChessRuleAdapter.h"
#include "ChessPieceSelectableState.h"

#include "ChessTextureCell.h"

#include "base/Logger.h"

namespace bgg
{
  ChessBoardView::ChessBoardView(
      sf::IntRect const &boardRect,
      std::shared_ptr<IChessRuleAdapter> gameRule,
      std::shared_ptr<TileMap> tileMap,
      std::shared_ptr<ItemStore> itemStore,
      std::function<void(ClientRequest const &)> requestSender) noexcept
      : stateStack_(),
        lastBoardState_(nullptr),
        gameRule_(std::move(gameRule)),
        tileMap_(std::move(tileMap)),
        itemStore_(std::move(itemStore)),
        requestSender_(std::move(requestSender))
  {
    BOOST_ASSERT_MSG(gameRule_, "gameRule_ of ChessBoardView cannot be null.");
    BOOST_ASSERT_MSG(tileMap_, "tileMap_ of ChessBoardView cannot be null.");
    BOOST_ASSERT_MSG(itemStore_, "itemStore_ of ChessBoardView cannot be null.");
    BOOST_ASSERT_MSG(requestSender_, "requestSender_ of ChessBoardView cannot be null.");

    fitRectangle(boardRect);

    ItemPlacementMap itemPlacements = gameRule_->getItemPlacements();
    for (auto const &[tile, entry] : itemPlacements)
    {
      tileMap_->fitItemToTile(entry.getItem(), tile);
    }

    persistentHighlighters_[0] = itemStore_->addItem(
        ZOrder::FIRST_LAYER,
        int(ChessTextureCell::LAST_MOVE_HIGHLIGHTER),
        utils::toString(ChessTextureCell::LAST_MOVE_HIGHLIGHTER),
        false);

    persistentHighlighters_[1] = itemStore_->addItem(
        ZOrder::FIRST_LAYER,
        int(ChessTextureCell::LAST_MOVE_HIGHLIGHTER),
        utils::toString(ChessTextureCell::LAST_MOVE_HIGHLIGHTER),
        false);

    persistentHighlighters_[2] = itemStore_->addItem(
        ZOrder::FIRST_LAYER,
        int(ChessTextureCell::CHECK_HIGHLIGHTER),
        utils::toString(ChessTextureCell::CHECK_HIGHLIGHTER),
        false);

    tileMap_->fitItemToTile(
        persistentHighlighters_[0].getItem(), sf::Vector2i{0, 0});
    persistentHighlighters_[0].getItem().setVisible(false);

    tileMap_->fitItemToTile(
        persistentHighlighters_[1].getItem(), sf::Vector2i{0, 0});
    persistentHighlighters_[1].getItem().setVisible(false);

    tileMap_->fitItemToTile(
        persistentHighlighters_[2].getItem(), sf::Vector2i{0, 0});
    persistentHighlighters_[2].getItem().setVisible(false);
  }

  void ChessBoardView::onWindowEvent(sf::Event const &event) noexcept
  {
    if (stateStack_.empty())
    {
      return;
    }

    auto &currentBoardState = stateStack_.back();

    if (lastBoardState_ != currentBoardState)
    {
      lastBoardState_ = currentBoardState;
    }

    if (auto mouseMoved = event.getIf<sf::Event::MouseMoved>())
    {
      currentBoardState->onMouseMoved(mouseMoved->position);
    }
    else if (auto mouseBtnPressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
      if (mouseBtnPressed->button == sf::Mouse::Button::Left)
      {
        currentBoardState->onMousePressed(mouseBtnPressed->position);
      }
    }
    else if (auto mouseBtnReleased = event.getIf<sf::Event::MouseButtonReleased>())
    {
      if (mouseBtnReleased->button == sf::Mouse::Button::Left)
      {
        currentBoardState->onMouseReleased(mouseBtnReleased->position);
      }
    }
  }

  void ChessBoardView::sendMoveRequest(MoveRequest const &move) noexcept
  {
    requestSender_(move);
  }

  void ChessBoardView::handleServerMessage(ServerMessage const &msg) noexcept
  {
    stateStack_.back()->onServerMessage(msg);
  }

  void ChessBoardView::draw(
      sf::RenderTarget &target, sf::RenderStates states) const noexcept
  {
    // draw the tileMap
    target.draw(*tileMap_, states);
    for (BoardItem &item : *itemStore_)
    {
      if (item.isVisible())
      {
        target.draw(item, states);
      }
    }
  }

  void ChessBoardView::changeState(std::shared_ptr<IBoardViewState> newState,
                                   sf::Vector2i const &mousePos) noexcept
  {
    BOOST_ASSERT_MSG(newState != nullptr, "State of ChessBoardView cannot be null");
    BOOST_ASSERT_MSG(
        !stateStack_.empty(),
        "You should not change board state while the state stack is empty");

    auto &lastState = stateStack_.back();
    lastState->onExit();
    lastState = std::move(newState);
    lastState->onEnter(mousePos);
  }

  void ChessBoardView::pushState(std::shared_ptr<IBoardViewState> newState,
                                 sf::Vector2i const &mousePos) noexcept
  {
    BOOST_ASSERT_MSG(newState != nullptr, "State of ChessBoardView cannot be null");

    if (!stateStack_.empty())
    {
      stateStack_.back()->onExit();
    }

    stateStack_.push_back(std::move(newState));
    stateStack_.back()->onEnter(mousePos);
  }

  void ChessBoardView::popState(sf::Vector2i const &mousePos) noexcept
  {
    BOOST_ASSERT_MSG(
        !stateStack_.empty(),
        "You should not pop a board state while the state stack is empty");

    stateStack_.back()->onExit();
    stateStack_.pop_back();

    if (!stateStack_.empty())
    {
      stateStack_.back()->onEnter(mousePos);
    }
  }

  void ChessBoardView::clearStates() noexcept
  {
    if (stateStack_.empty())
    {
      return;
    }

    stateStack_.back()->onExit();
    stateStack_.clear();
  }

  void ChessBoardView::fitRectangle(sf::IntRect const &boardRect) noexcept
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
