// ChessPieceMovedState.cpp

#include <boost/assert.hpp>

#include "ChessPieceMovedState.h"

#include "base/Logger.h"
#include "ItemMove.h"

namespace bgg
{
  ChessPieceMovedState::ChessPieceMovedState(
      std::shared_ptr<IChessBoard> gameBoard,
      std::shared_ptr<IChessRuleAdapter> gameRule,
      std::shared_ptr<TileMap> tileMap,
      std::shared_ptr<ItemStore> itemStore,
      ItemMove const &move) noexcept
      : gameBoard_(std::move(gameBoard)),
        gameRule_(std::move(gameRule)),
        tileMap_(std::move(tileMap)),
        itemStore_(std::move(itemStore))
  {
  }

  void ChessPieceMovedState::onEnter(sf::Vector2i const &mousePos) noexcept
  {

    //==========
    SPDLOG_INFO("Entered '{}'", typeid(*this).name());
    //==========
  }

  void ChessPieceMovedState::onExit() noexcept
  {
  }

  void ChessPieceMovedState::onMouseMoved(sf::Vector2i const & /*mousePos*/) noexcept
  {
    // do nothing
  }

  void ChessPieceMovedState::onMousePressed(sf::Vector2i const & /*mousePos*/) noexcept
  {
    // do nothing
  }

  void ChessPieceMovedState::onMouseReleased(sf::Vector2i const & /*mousePos*/) noexcept
  {
    // do nothing
  }

  void ChessPieceMovedState::onServerMessage(ServerMessage const &msg) noexcept
  {
    if (auto gameUpdatedNotif = msg.getIf<GameUpdatedNotification>())
    {
      // onGameUpdatedNotification(*gameUpdatedNotif);
    }
    else if (auto commitMoveResponse = msg.getIf<MoveResponse>())
    {
      // onMoveResponse(*commitMoveResponse);
    }
    else
    {
      BOOST_ASSERT_MSG(
          false, "This handler is not for this kind of message");
    }
  }

  // void ChessBoard::requestMove(ChessMove const &move) noexcept
  // {
  //   // add highlighters for the new move
  //   tileMap_->fitItemToTile(lastMoveHighlighters_[0].getItem(), move.fromTile);
  //   tileMap_->fitItemToTile(lastMoveHighlighters_[1].getItem(), move.toTile);

  //   auto movedItemEntry = gameRule_->getItemEntry(move.fromTile);
  //   BOOST_ASSERT_MSG(
  //       !movedItemEntry.isNull(),
  //       "There must be an item at the 'fromTile'");

  //   if (move.promote)
  //   {
  //     auto itemColor = gameRule_->getPieceColor(move.fromTile);
  //     Piece promotedPiece{move.promote.value(), itemColor.value()};

  //     // add new promoted item to itemStore_
  //     pendingPromotedItem_ = itemStore_->addItem(
  //         ZOrder::SECOND_LAYER,
  //         ChessTextureCell::getIndex(promotedPiece),
  //         promotedPiece.toString());

  //     // fit the new promoted item to 'toTile', make moved item invisible but not move it
  //     tileMap_->fitItemToTile(pendingPromotedItem_.getItem(), move.toTile);
  //     movedItemEntry.getItem().setVisible(false);
  //   }
  //   else
  //   {
  //     tileMap_->fitItemToTile(movedItemEntry.getItem(), move.toTile);
  //   }

  //   // make captured item invisible
  //   auto capturedItemEntry = gameRule_->getItemEntry(move.toTile);
  //   if (!capturedItemEntry.isNull())
  //   {
  //     capturedItemEntry.getItem().setVisible(false);
  //   }

  //   //
  //   pendingMove_ = move;

  //   requestSender_(MoveRequest{
  //       "",
  //       "",
  //       gameRule_->tileToPosition(move.fromTile),
  //       gameRule_->tileToPosition(move.toTile),
  //       move.promote});

  //   // BoardItem &selectedItem = selectedItemEntry_.getItem();
  // }

  /////////////////////////////////////////////////////////////////////////

  // void ChessBoard::onGameUpdatedNotification(
  //     GameUpdatedNotification const &notif) noexcept
  // {
  //   // TODO: ChessBoard::onGameUpdatedNotification()
  //   ChessMove move{
  //       gameRule_->positionToTile(notif.fromSquare), // fromTile
  //       gameRule_->positionToTile(notif.toSquare),   // toTile
  //       notif.promote};

  //   lastMoveTiles_[0] = move.fromTile;
  //   lastMoveTiles_[1] = move.toTile;

  //   tileMap_->fitItemToTile(lastMoveHighlighters_[0].getItem(), move.fromTile);
  //   tileMap_->fitItemToTile(lastMoveHighlighters_[1].getItem(), move.toTile);

  //   auto movedItemEntry = gameRule_->getItemEntry(move.fromTile);
  //   BOOST_ASSERT_MSG(
  //       !movedItemEntry.isNull(),
  //       "There must be an item at the 'fromTile'");

  //   std::optional<BoardItem> promotedItem(std::nullopt);
  //   if (move.promote)
  //   {
  //     auto itemColor = gameRule_->getItemColor(move.fromTile);
  //     Piece promotedPiece{move.promote.value(), itemColor.value()};

  //     promotedItem = itemStore_->createItem(
  //         ChessTextureCell::getIndex(promotedPiece),
  //         promotedPiece.toString());
  //   }

  //   ItemStore::Entry capturedItemEntry = gameRule_->commitMove(move, promotedItem);
  //   // remove capturedItem from itemStore_ even if any or not
  //   itemStore_->removeItem(capturedItemEntry);

  //   // fit the moved item to 'toTile'
  //   tileMap_->fitItemToTile(movedItemEntry.getItem(), move.toTile);

  //   if (notif.yourTurn == notif.currentTurn)
  //   {
  //     std::shared_ptr<IBoardState>
  //         nextBoardState = std::make_shared<ChessPieceSelectableState>(
  //             shared_from_this(), gameRule_, tileMap_, itemStore_);

  //     pushState(std::move(nextBoardState));
  //   }

  //   SPDLOG_INFO("A message of type '{}' has been handled by '{}'",
  //               typeid(notif).name(), typeid(*this).name());
  // }

  /////////////////////////////////////////////////////////////////////////

  // void ChessBoard::onMoveResponse(MoveResponse const &response) noexcept
  // {
  //   if (response.errcode.failed())
  //   {
  //     // restore lastMoveHighlighters_ to the previous tiles
  //     if (lastMoveTiles_[0] && lastMoveTiles_[1])
  //     {
  //       TileCoords fromTile = lastMoveTiles_[0].value();
  //       TileCoords toTile = lastMoveTiles_[1].value();
  //       tileMap_->fitItemToTile(lastMoveHighlighters_[0].getItem(), fromTile);
  //       tileMap_->fitItemToTile(lastMoveHighlighters_[1].getItem(), toTile);
  //     }
  //     else
  //     {
  //       lastMoveHighlighters_[0].getItem().setVisible(false);
  //       lastMoveHighlighters_[1].getItem().setVisible(false);
  //     }

  //     auto movedItemEntry = gameRule_->getItemEntry(pendingMove_.fromTile);
  //     BOOST_ASSERT_MSG(
  //         !movedItemEntry.isNull(),
  //         "There must be an item at the 'pendingMove_.fromTile'");

  //     // fit the moved item to 'fromTile'
  //     tileMap_->fitItemToTile(movedItemEntry.getItem(), pendingMove_.fromTile);
  //     if (!pendingPromotedItem_.isNull())
  //     {
  //       // make moved item visible
  //       movedItemEntry.getItem().setVisible(true);
  //     }

  //     // make captured item visible
  //     auto capturedItemEntry = gameRule_->getItemEntry(pendingMove_.toTile);
  //     if (!capturedItemEntry.isNull())
  //     {
  //       capturedItemEntry.getItem().setVisible(true);
  //     }

  //     // revert to ChessPiece SelectableState
  //     std::shared_ptr<IBoardState>
  //         nextBoardState = std::make_shared<ChessPieceSelectableState>(
  //             shared_from_this(), gameRule_, tileMap_, itemStore_);

  //     pushState(std::move(nextBoardState));
  //   }
  //   else
  //   {
  //     // update the pending move to game rule
  //     std::optional<BoardItem> promotedItem(std::nullopt);
  //     if (!pendingPromotedItem_.isNull())
  //     {
  //       promotedItem = std::move(pendingPromotedItem_.getItem());
  //     }

  //     ItemStore::Entry capturedItemEntry = gameRule_->commitMove(
  //         pendingMove_, std::move(promotedItem));

  //     // remove the captured item even if any or not
  //     itemStore_->removeItem(capturedItemEntry);
  //   }

  //   pendingMove_ = ChessMove{{-1, -1}, {-1, -1}, std::nullopt};

  //   // remove the new promoted item from itemStore_ even if any or not
  //   itemStore_->removeItem(pendingPromotedItem_);
  //   pendingPromotedItem_ = ItemStore::Entry();

  //   SPDLOG_INFO("A message of type '{}' has been handled by '{}'",
  //               typeid(response).name(), typeid(*this).name());
  // }
} // namespace bgg
