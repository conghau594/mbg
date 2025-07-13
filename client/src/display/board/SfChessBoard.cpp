// SfChessBoard.cpp

#include <list>

#include <boost/assert.hpp>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "SfChessBoard.h"
#include "SfBoardState.h"
#include "SfGameRuleAdapter.h"

#include "ChessTextureCell.h"

#include "base/Logger.h"

namespace bgg
{
  SfChessBoard::SfChessBoard(
      sf::IntRect const &boardRect,
      std::shared_ptr<SfGameRuleAdapter> gameRule,
      std::shared_ptr<SfTileMap> tileMap,
      std::shared_ptr<SfItemStore> itemStore,
      std::function<void(ClientRequest const &)> requestSender) noexcept
      : stateStack_(),
        lastBoardState_(nullptr),
        gameRule_(std::move(gameRule)),
        tileMap_(std::move(tileMap)),
        itemStore_(std::move(itemStore)),
        requestSender_(std::move(requestSender)),
        lastMoveHighlighters_{{*itemStore_}, {*itemStore_}}
  {
    BOOST_ASSERT_MSG(gameRule_, "gameRule_ of SfChessBoard cannot be null.");
    BOOST_ASSERT_MSG(tileMap_, "tileMap_ of SfChessBoard cannot be null.");
    BOOST_ASSERT_MSG(itemStore_, "itemStore_ of SfChessBoard cannot be null.");
    BOOST_ASSERT_MSG(requestSender_, "requestSender_ of SfChessBoard cannot be null.");

    fitRectangle(boardRect);

    SfItemPlacementMap itemPlacements = gameRule_->getItemPlacements();
    for (auto &[tile, entry] : itemPlacements)
    {
      tileMap_->fitItemToTile(entry.getItem(), tile);
    }

    lastMoveHighlighters_[0] = itemStore_->addItem(
        ChessTextureCell::LAST_MOVE_HIGHLIGHTER,
        ZOrder::FIRST_LAYER,
        ChessTextureCell::toString(ChessTextureCell::LAST_MOVE_HIGHLIGHTER).value(),
        false);

    lastMoveHighlighters_[1] = itemStore_->addItem(
        ChessTextureCell::LAST_MOVE_HIGHLIGHTER,
        ZOrder::FIRST_LAYER,
        ChessTextureCell::toString(ChessTextureCell::LAST_MOVE_HIGHLIGHTER).value(),
        false);
  }

  void SfChessBoard::onEvent(sf::Event const &event) noexcept
  {
    if (stateStack_.empty())
    {
      return;
    }

    BOOST_ASSERT_MSG(stateStack_.back() != nullptr,
                     "The current board state of SfChessBoard cannot be null");

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

  void SfChessBoard::commitAction(BoardAction const &action) noexcept
  {
    if (auto const &pieceMove = action.getIf<MoveChessPiece>())
    {
      tileMap_->fitItemToTile(
          lastMoveHighlighters_[0].getItem(), pieceMove->fromSquare);

      tileMap_->fitItemToTile(
          lastMoveHighlighters_[1].getItem(), pieceMove->toSquare);

      bool somethingWrong = true;
      if (auto selectedItemEntry = gameRule_->getItemEntry(pieceMove->fromSquare))
      {
        tileMap_->fitItemToTile(
            selectedItemEntry.value().getItem(), pieceMove->toSquare);

        somethingWrong = false;
      }

      BOOST_ASSERT_MSG(
          !somethingWrong, "Something wrong: there is no item at the selectedTile_");

      requestSender_(CommitMoveRequest{
          "",
          "",
          // gameRule_->getSide(),
          {pieceMove->fromSquare.x, pieceMove->fromSquare.y},
          {pieceMove->toSquare.x, pieceMove->toSquare.y}});

      // SfBoardItem &selectedItem = selectedItemEntry_.getItem();
    }
    // TODO: SfChessBoard::send(ClientRequest const &request) noexcept
  }

  void SfChessBoard::handleServerMessage(ServerMessage const &msg) noexcept
  {
    if (auto gameUpdatedNotif = msg.getIf<GameUpdatedNotification>())
    {
      onGameUpdatedNotification(*gameUpdatedNotif);
    }
    else if (auto gameFinishedNotif = msg.getIf<GameFinishedNotification>())
    {
      onGameFinishedNotification(*gameFinishedNotif);
    }
    else if (auto commitMoveResponse = msg.getIf<CommitMoveResponse>())
    {
      onCommitMoveResponse(*commitMoveResponse);
    }
    else
    {
      BOOST_ASSERT_MSG(
          false, "This function is not for this kind of message");
    }
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

  void SfChessBoard::changeState(std::shared_ptr<SfBoardState> newState,
                                 sf::Vector2i const &mousePos) noexcept
  {
    BOOST_ASSERT_MSG(
        !stateStack_.empty(),
        "You should not change board state while the state stack is empty");

    auto &lastState = stateStack_.back();
    lastState->onExit();
    lastState = std::move(newState);
    lastState->onEnter(mousePos);
  }

  void SfChessBoard::pushState(std::shared_ptr<SfBoardState> newState,
                               sf::Vector2i const &mousePos) noexcept
  {
    if (!stateStack_.empty())
    {
      stateStack_.back()->onExit();
    }

    stateStack_.push_back(std::move(newState));
    stateStack_.back()->onEnter(mousePos);
  }

  void SfChessBoard::popState(sf::Vector2i const &mousePos) noexcept
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

  void SfChessBoard::clearStates() noexcept
  {
    if (stateStack_.empty())
    {
      return;
    }

    stateStack_.back()->onExit();
    stateStack_.clear();
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

  void SfChessBoard::onGameUpdatedNotification(
      GameUpdatedNotification const &notif) noexcept
  {

    SPDLOG_INFO("A message of type '{}' has been handled by '{}'",
                typeid(notif).name(), typeid(*this).name());
  }

  void SfChessBoard::onGameFinishedNotification(
      GameFinishedNotification const &notif) noexcept
  {

    SPDLOG_INFO("A message of type '{}' has been handled by '{}'",
                typeid(notif).name(), typeid(*this).name());
  }

  void SfChessBoard::onCommitMoveResponse(
      CommitMoveResponse const &response) noexcept
  {

    SPDLOG_INFO("A message of type '{}' has been handled by '{}'",
                typeid(response).name(), typeid(*this).name());
  }
} // namespace bgg
