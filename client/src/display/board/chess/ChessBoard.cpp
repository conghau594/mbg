// ChessBoard.cpp

#include <list>

#include <boost/assert.hpp>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "ChessBoard.h"
#include "display/board/IBoardState.h"
#include "IChessRuleAdapter.h"
#include "ChessPieceSelectableState.h"

#include "ChessTextureCell.h"

#include "base/Logger.h"

namespace bgg
{
  ChessBoard::ChessBoard(
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
        requestSender_(std::move(requestSender)),
        lastMoveTiles_{std::nullopt, std::nullopt},
        pendingMove_{{-1, -1}, {-1, -1}, std::nullopt},
        promotionItem_{},
        lastMoveHighlighters_{{}, {}}
  {
    BOOST_ASSERT_MSG(gameRule_, "gameRule_ of ChessBoard cannot be null.");
    BOOST_ASSERT_MSG(tileMap_, "tileMap_ of ChessBoard cannot be null.");
    BOOST_ASSERT_MSG(itemStore_, "itemStore_ of ChessBoard cannot be null.");
    BOOST_ASSERT_MSG(requestSender_, "requestSender_ of ChessBoard cannot be null.");

    fitRectangle(boardRect);

    ItemPlacementMap itemPlacements = gameRule_->getItemPlacements();
    for (auto const &[tile, entry] : itemPlacements)
    {
      tileMap_->fitItemToTile(entry.getItem(), tile);
    }

    lastMoveHighlighters_[0] = itemStore_->addItem(
        ChessTextureCell::LAST_MOVE_HIGHLIGHTER,
        ZOrder::FIRST_LAYER,
        ChessTextureCell::toString(ChessTextureCell::LAST_MOVE_HIGHLIGHTER),
        false);

    lastMoveHighlighters_[1] = itemStore_->addItem(
        ChessTextureCell::LAST_MOVE_HIGHLIGHTER,
        ZOrder::FIRST_LAYER,
        ChessTextureCell::toString(ChessTextureCell::LAST_MOVE_HIGHLIGHTER),
        false);
  }

  void ChessBoard::onWindowEvent(sf::Event const &event) noexcept
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

  void ChessBoard::handleServerMessage(ServerMessage const &msg) noexcept
  {
    if (auto gameUpdatedNotif = msg.getIf<GameUpdatedNotification>())
    {
      onGameUpdatedNotification(*gameUpdatedNotif);
    }
    else if (auto gameFinishedNotif = msg.getIf<GameFinishedNotification>())
    {
      onGameFinishedNotification(*gameFinishedNotif);
    }
    else if (auto commitMoveResponse = msg.getIf<MoveResponse>())
    {
      onMoveResponse(*commitMoveResponse);
    }
    else
    {
      BOOST_ASSERT_MSG(
          false, "This handler is not for this kind of message");
    }
  }

  void ChessBoard::draw(
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

  void ChessBoard::changeState(std::shared_ptr<IBoardState> newState,
                               sf::Vector2i const &mousePos) noexcept
  {
    BOOST_ASSERT_MSG(newState != nullptr, "State of ChessBoard cannot be null");
    BOOST_ASSERT_MSG(
        !stateStack_.empty(),
        "You should not change board state while the state stack is empty");

    auto &lastState = stateStack_.back();
    lastState->onExit();
    lastState = std::move(newState);
    lastState->onEnter(mousePos);
  }

  void ChessBoard::pushState(std::shared_ptr<IBoardState> newState,
                             sf::Vector2i const &mousePos) noexcept
  {
    BOOST_ASSERT_MSG(newState != nullptr, "State of ChessBoard cannot be null");

    if (!stateStack_.empty())
    {
      stateStack_.back()->onExit();
    }

    stateStack_.push_back(std::move(newState));
    stateStack_.back()->onEnter(mousePos);
  }

  void ChessBoard::popState(sf::Vector2i const &mousePos) noexcept
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

  void ChessBoard::clearStates() noexcept
  {
    if (stateStack_.empty())
    {
      return;
    }

    stateStack_.back()->onExit();
    stateStack_.clear();
  }

  void ChessBoard::requestMove(ChessPieceMove const &move) noexcept
  {
    tileMap_->fitItemToTile(lastMoveHighlighters_[0].getItem(), move.fromTile);
    tileMap_->fitItemToTile(lastMoveHighlighters_[1].getItem(), move.toTile);

    moveItemEntry(move);
    pendingMove_ = move;

    requestSender_(MoveRequest{
        "",
        "",
        gameRule_->tileToPosition(move.fromTile),
        gameRule_->tileToPosition(move.toTile),
        move.promote});

    // BoardItem &selectedItem = selectedItemEntry_.getItem();
  }

  void ChessBoard::fitRectangle(sf::IntRect const &boardRect) noexcept
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

  void ChessBoard::moveItemEntry(ChessPieceMove const &move) noexcept
  {
    tileMap_->fitItemToTile(lastMoveHighlighters_[0].getItem(), move.fromTile);
    tileMap_->fitItemToTile(lastMoveHighlighters_[1].getItem(), move.toTile);

    auto movedItemEntry = gameRule_->getItemEntry(move.fromTile);
    BOOST_ASSERT_MSG(!movedItemEntry.isNull(), "There must be an item at the 'fromTile'");

    if (move.promote)
    {
      auto itemColor = gameRule_->getColor(move.fromTile);
      Piece promotedPiece{move.promote.value(), itemColor.value()};

      promotionItem_ = itemStore_->addItem(
          ChessTextureCell::getIndex(promotedPiece),
          ZOrder::SECOND_LAYER,
          promotedPiece.toString());

      tileMap_->fitItemToTile(promotionItem_.getItem(), move.toTile);

      movedItemEntry.getItem().setVisible(false);
    }
    else
    {
      tileMap_->fitItemToTile(movedItemEntry.getItem(), move.toTile);
    }

    auto capturedItemEntry = gameRule_->getItemEntry(move.toTile);
    if (!capturedItemEntry.isNull())
    {
      capturedItemEntry.getItem().setVisible(false);
    }
  }

  void ChessBoard::onGameUpdatedNotification(
      GameUpdatedNotification const &notif) noexcept
  {
    TileCoords fromTile = gameRule_->positionToTile(notif.fromPosition);
    TileCoords toTile = gameRule_->positionToTile(notif.toPosition);

    ChessPieceMove move{fromTile, toTile, notif.promote};
    moveItemEntry(move);

    lastMoveTiles_[0] = fromTile;
    lastMoveTiles_[1] = toTile;

    gameRule_->commitMove(move);
    // TODO: remove capturedItem from itemStore_

    if (notif.yourTurn == notif.currentTurn)
    {
      std::shared_ptr<IBoardState>
          nextBoardState = std::make_shared<ChessPieceSelectableState>(
              shared_from_this(), gameRule_, tileMap_, itemStore_);

      pushState(std::move(nextBoardState), {-1000, -1000}); /// mousePos is far away from the window
    }

    SPDLOG_INFO("A message of type '{}' has been handled by '{}'",
                typeid(notif).name(), typeid(*this).name());
  }

  void ChessBoard::onGameFinishedNotification(
      GameFinishedNotification const &notif) noexcept
  {
    // TODO: ChessBoard::onGameFinishedNotification()

    SPDLOG_INFO("A message of type '{}' has been handled by '{}'",
                typeid(notif).name(), typeid(*this).name());
  }

  void ChessBoard::onMoveResponse(MoveResponse const &response) noexcept
  {
    if (response.errcode.failed())
    {
      // revert to the previous state
      if (lastMoveTiles_[0] && lastMoveTiles_[1])
      {
        TileCoords fromTile = lastMoveTiles_[0].value();
        TileCoords toTile = lastMoveTiles_[1].value();
        tileMap_->fitItemToTile(lastMoveHighlighters_[0].getItem(), fromTile);
        tileMap_->fitItemToTile(lastMoveHighlighters_[1].getItem(), toTile);
      }
      else
      {
        lastMoveHighlighters_[0].getItem().setVisible(false);
        lastMoveHighlighters_[1].getItem().setVisible(false);
      }

      auto selectedItemEntry = gameRule_->getItemEntry(pendingMove_.fromTile);
      BOOST_ASSERT_MSG(
          !selectedItemEntry.isNull(),
          "There must be an item at the 'pendingMoveTiles_[0]'");

      tileMap_->fitItemToTile(selectedItemEntry.getItem(), pendingMove_.toTile);

      auto capturedItemEntry = gameRule_->getItemEntry(pendingMove_.toTile);
      if (!capturedItemEntry.isNull())
      {
        capturedItemEntry.getItem().setVisible(true);
      }

      if (!promotionItem_.isNull())
      {
        itemStore_->removeItem(promotionItem_);
      }

      std::shared_ptr<IBoardState>
          nextBoardState = std::make_shared<ChessPieceSelectableState>(
              shared_from_this(), gameRule_, tileMap_, itemStore_);

      pushState(std::move(nextBoardState), {-1000, -1000});
    }
    else
    {
      // update the pending move to game rule
      gameRule_->commitMove(pendingMove_);

      // TODO: remove capturedItem from itemStore_
    }

    SPDLOG_INFO("A message of type '{}' has been handled by '{}'",
                typeid(response).name(), typeid(*this).name());
  }
} // namespace bgg
