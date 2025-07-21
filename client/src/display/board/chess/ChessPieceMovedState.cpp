// ChessPieceMovedState.cpp

#include <boost/assert.hpp>

#include "ChessPieceMovedState.h"
#include "IChessRuleAdapter.h"

#include "base/Logger.h"
#include "display/board/TileMap.h"
#include "display/board/ZOrder.h"
#include "service/ClientRequest.h"

#include "IChessBoard.h"
#include "ItemMove.h"
#include "ChessTextureCell.h"
#include "IChessRuleAdapter.h"

namespace bgg
{
  ChessPieceMovedState::ChessPieceMovedState(
      std::shared_ptr<IChessBoard> gameBoard,
      std::shared_ptr<IChessRuleAdapter> gameRule,
      std::shared_ptr<TileMap> tileMap,
      std::shared_ptr<ItemStore> itemStore,
      std::optional<ItemMove> const &move) noexcept
      : gameBoard_(std::move(gameBoard)),
        gameRule_(std::move(gameRule)),
        tileMap_(std::move(tileMap)),
        itemStore_(std::move(itemStore)),
        checkHighlighter_(),
        pendingPromotionItem_(),
        pendingMoveHighlighters_{},
        lastMoveHighlighters_{nullptr, nullptr},
        pendingItemMoveAction_{
            move ? gameRule_->tryMove(*move) : ChessItemMoveAction{}}
  {
    if (!pendingItemMoveAction_.getIf<std::monostate>() &&
        !pendingItemMoveAction_.getIf<InvalidChessItemMove>())
    {
      gameBoard_->sendMoveRequest(MoveRequest{
          "",
          "",
          gameRule_->tileToPosition(move->fromTile),
          gameRule_->tileToPosition(move->toTile),
          move->promote});
    }
  }

  void ChessPieceMovedState::onEnter(sf::Vector2i const & /*mousePos*/) noexcept
  {
    std::list<BoardItem *> lastMoveHighlighterList = itemStore_->findItems(
        [](BoardItem const &item)
        {
          std::string const lastMoveHighligherName = utils::toString(
              ChessTextureCell::LAST_MOVE_HIGHLIGHTER);
          return item.getName() == lastMoveHighligherName;
        });

    BOOST_ASSERT_MSG(
        lastMoveHighlighterList.size() == 2,
        "There must be exactly two lastMoveHighlighters_ in the itemStore_");

    lastMoveHighlighters_[0] = lastMoveHighlighterList.front();
    lastMoveHighlighters_[1] = lastMoveHighlighterList.back();
    lastMoveHighlighterVisibility_[0] = lastMoveHighlighters_[0]->isVisible();
    lastMoveHighlighterVisibility_[1] = lastMoveHighlighters_[1]->isVisible();

    pendingMoveHighlighters_[0] = itemStore_->addItem(
        ZOrder::FIRST_LAYER,
        int(ChessTextureCell::LAST_MOVE_HIGHLIGHTER),
        utils::toString(ChessTextureCell::LAST_MOVE_HIGHLIGHTER),
        false);

    pendingMoveHighlighters_[1] = itemStore_->addItem(
        ZOrder::FIRST_LAYER,
        int(ChessTextureCell::LAST_MOVE_HIGHLIGHTER),
        utils::toString(ChessTextureCell::LAST_MOVE_HIGHLIGHTER),
        false);

    if (!pendingItemMoveAction_.isEmpty())
    {
      updateBoard(pendingItemMoveAction_);
    }
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
      onGameUpdatedNotification(*gameUpdatedNotif);
    }
    else if (auto moveResponse = msg.getIf<MoveResponse>())
    {
      onMoveResponse(*moveResponse);
    }
    else
    {
      BOOST_ASSERT_MSG(false, "This handler is not for this kind of message");
    }
  }

  void ChessPieceMovedState::updateBoard(
      ChessItemMoveAction const &itemMoveAction) noexcept
  {
    auto moveActionVisitor = [this]<typename T>(T const &action)
    {
      if constexpr (requires {
          { action.fromTile } -> std::same_as<TileCoords const &>;
          { action.toTile } -> std::same_as<TileCoords const &>;
          { action.enemyKingTile } -> std::same_as<TileCoords const &>;
          { action.enemyKingState } -> std::same_as<KingState const &>; })
      {
        auto movedItemEntry = gameRule_->getItemEntry(action.fromTile);
        BOOST_ASSERT_MSG(
            !movedItemEntry.isNull(),
            "There must be an item at the 'fromTile'");

        previewBasicMoveAction(
            movedItemEntry.getItem(),
            action.fromTile,
            action.toTile,
            action.enemyKingTile,
            action.enemyKingState);

        if constexpr (requires { 
            { action.promote } -> std::same_as<std::string const &>; }) ///< if constexpr (std::is_same_v<T, ChessPromotionItemMove>)
        {
          auto itemColor = gameRule_->getItemColor(action.fromTile);
          Piece promotedPiece{action.promote, itemColor.value()};

          // add new promoted item to itemStore_
          pendingPromotionItem_ = itemStore_->addItem(
              ZOrder::SECOND_LAYER,
              int(utils::getChessTextureCellIndex(promotedPiece)),
              promotedPiece.toString());

          // fit the promoted item to 'toTile', make moved item invisible but not move it
          tileMap_->fitItemToTile(pendingPromotionItem_.getItem(), action.toTile);
          movedItemEntry.getItem().setVisible(false);
        }
        else if constexpr (requires { 
            { action.enPassantTile } -> std::same_as<TileCoords const &>; }) ///< if constexpr (std::is_same_v<T, ChessEnPassantItemMove>)
        {
          // make en passant captured item invisible
          auto enPassantItemEntry = gameRule_->getItemEntry(action.enPassantTile);
          BOOST_ASSERT_MSG(
              !enPassantItemEntry.isNull(),
              "There must be an item at the action.enPassantTile");

          enPassantItemEntry.getItem().setVisible(false);
        }
        else if constexpr (requires { 
                { action.rookSource } -> std::same_as<TileCoords const &>;
                { action.rookDestination } -> std::same_as<TileCoords const &>; }) ///< if constexpr (std::is_same_v<T, ChessCastlingItemMove>)
        {
          // move the related rook to the destination
          auto rookItemEntry = gameRule_->getItemEntry(action.rookSource);
          BOOST_ASSERT_MSG(
              !rookItemEntry.isNull(),
              "There must be an item at the tile of action.rookSource");

          tileMap_->fitItemToTile(rookItemEntry.getItem(), action.rookDestination);
        }

        SPDLOG_INFO("You have tried to do a '{}'", typeid(T).name());
      }
      else if constexpr (std::is_same_v<T, std::monostate>)
      {
        SPDLOG_WARN("You have tried to do an empty move");
      }
      else if constexpr (std::is_same_v<T, InvalidChessItemMove>)
      {
        SPDLOG_WARN("You have tryied to do an 'InvalidChessItemMove'");
      }
    };

    itemMoveAction.visit(moveActionVisitor);
  }

  void ChessPieceMovedState::previewBasicMoveAction(
      BoardItem &movedItemEntry,
      TileCoords const &fromTile,
      TileCoords const &toTile,
      TileCoords const &enemyKingTile,
      KingState const &enemyKingState) noexcept
  {
    // add highlighters for the new move
    lastMoveHighlighters_[0]->setVisible(false);
    lastMoveHighlighters_[1]->setVisible(false);
    tileMap_->fitItemToTile(pendingMoveHighlighters_[0].getItem(), fromTile);
    tileMap_->fitItemToTile(pendingMoveHighlighters_[1].getItem(), toTile);

    tileMap_->fitItemToTile(movedItemEntry, toTile);

    // make captured item invisible
    auto capturedItemEntry = gameRule_->getItemEntry(toTile);
    if (!capturedItemEntry.isNull())
    {
      capturedItemEntry.getItem().setVisible(false);
    }

    if (enemyKingState == KingState::IN_CHECK)
    {
      checkHighlighter_ = itemStore_->addItem(
          ZOrder::FIRST_LAYER,
          int(ChessTextureCell::CHECK_HIGHLIGHTER),
          utils::toString(ChessTextureCell::CHECK_HIGHLIGHTER));
    }
    else if (enemyKingState == KingState::CHECKMATED)
    {
      checkHighlighter_ = itemStore_->addItem(
          ZOrder::THIRD_LAYER,
          int(ChessTextureCell::CHECKMATE_HIGHLIGHTER),
          utils::toString(ChessTextureCell::CHECKMATE_HIGHLIGHTER));
    }
    else
    {
      return;
    }

    tileMap_->fitItemToTile(checkHighlighter_.getItem(), enemyKingTile);
  }

  void ChessPieceMovedState::revertBasicMoveAction(
      BoardItem &movedItemEntry,
      TileCoords const &fromTile,
      TileCoords const &toTile) noexcept
  {
    // restore lastMoveHighlighters_ to the previous tiles
    lastMoveHighlighters_[0]->setVisible(lastMoveHighlighterVisibility_[0]);
    lastMoveHighlighters_[1]->setVisible(lastMoveHighlighterVisibility_[1]);

    itemStore_->removeItem(pendingMoveHighlighters_[0]);
    itemStore_->removeItem(pendingMoveHighlighters_[1]);

    // fit the moved item to 'fromTile'
    tileMap_->fitItemToTile(movedItemEntry, fromTile);

    // make captured item visible
    auto capturedItemEntry = gameRule_->getItemEntry(toTile);
    if (!capturedItemEntry.isNull())
    {
      capturedItemEntry.getItem().setVisible(true);
    }

    itemStore_->removeItem(checkHighlighter_);
  }

  void ChessPieceMovedState::finalizeBasicMoveAction(
      TileCoords const &fromTile,
      TileCoords const &toTile) noexcept
  {
    // fit lastMoveHighlighters_ to fromTile and toTile,
    // remove the temporary pendingMoveHighlighters_
    tileMap_->fitItemToTile(*(lastMoveHighlighters_[0]), fromTile);
    tileMap_->fitItemToTile(*(lastMoveHighlighters_[1]), toTile);

    itemStore_->removeItem(pendingMoveHighlighters_[0]);
    itemStore_->removeItem(pendingMoveHighlighters_[1]);

    // remove captured item from itemStore_
    auto capturedItemEntry = gameRule_->getItemEntry(toTile);
    if (!capturedItemEntry.isNull())
    {
      itemStore_->removeItem(capturedItemEntry);
    }
  }

  void ChessPieceMovedState::onMoveResponse(MoveResponse const &response) noexcept
  {
    if (response.errcode.failed())
    {
      auto moveActionVisitor = [this]<typename T>(T const &action)
      {
        if constexpr (requires {
          { action.fromTile } -> std::same_as<TileCoords const &>;
          { action.toTile } -> std::same_as<TileCoords const &>; })
        {
          auto movedItemEntry = gameRule_->getItemEntry(action.fromTile);
          BOOST_ASSERT_MSG(
              !movedItemEntry.isNull(),
              "There must be an item at the 'fromTile'");

          revertBasicMoveAction(
              movedItemEntry.getItem(),
              action.fromTile,
              action.toTile);

          if constexpr (requires { 
            { action.promote } -> std::same_as<std::string const &>; }) ///< if constexpr (std::is_same_v<T, ChessPromotionItemMove>)
          {
            // make the moved item visible and remove the pendingPromotionItem_
            movedItemEntry.getItem().setVisible(true);
            itemStore_->removeItem(pendingPromotionItem_);
          }
          else if constexpr (requires { 
            { action.enPassantTile } -> std::same_as<TileCoords const &>; }) ///< if constexpr (std::is_same_v<T, ChessEnPassantItemMove>)
          {
            // make en passant captured item visible
            auto enPassantItemEntry = gameRule_->getItemEntry(action.enPassantTile);
            BOOST_ASSERT_MSG(
                !enPassantItemEntry.isNull(),
                "There must be an item at the action.enPassantTile");

            enPassantItemEntry.getItem().setVisible(true);
          }
          else if constexpr (requires { 
                { action.rookSource } -> std::same_as<TileCoords const &>;
                { action.rookDestination } -> std::same_as<TileCoords const &>; }) ///< if constexpr (std::is_same_v<T, ChessCastlingItemMove>)
          {
            // move the related rook to the source
            auto rookItemEntry = gameRule_->getItemEntry(action.rookSource);
            BOOST_ASSERT_MSG(
                !rookItemEntry.isNull(),
                "There must be an item at the tile of action.rookSource");

            tileMap_->fitItemToTile(rookItemEntry.getItem(), action.rookSource);
          }

          SPDLOG_INFO("You have reverted a '{}'", typeid(T).name());
        }
        else if constexpr (std::is_same_v<T, std::monostate>)
        {
          SPDLOG_WARN("You have reverted an empty move");
        }
        else if constexpr (std::is_same_v<T, InvalidChessItemMove>)
        {
          SPDLOG_WARN("You have reverted an 'InvalidChessItemMove'");
        }
      };

      pendingItemMoveAction_.visit(moveActionVisitor);
      // revert to ChessPiece SelectableState
      gameBoard_->popState();
    }
    else
    {
      // update the pending move to game rule
      auto moveActionVisitor = [this]<typename T>(T const &action)
      {
        if constexpr (requires {
          { action.fromTile } -> std::same_as<TileCoords const &>;
          { action.toTile } -> std::same_as<TileCoords const &>; })
        {
          finalizeBasicMoveAction(action.fromTile, action.toTile);

          if constexpr (requires { 
            { action.promote } -> std::same_as<std::string const &>; }) ///< if constexpr (std::is_same_v<T, ChessPromotionItemMove>)
          {
            // remove the moved item from itemStore
            auto movedItemEntry = gameRule_->getItemEntry(action.fromTile);
            BOOST_ASSERT_MSG(
                !movedItemEntry.isNull(),
                "There must be an item at the 'fromTile'");

            movedItemEntry.getItem() = pendingPromotionItem_.getItem();
            itemStore_->removeItem(pendingPromotionItem_);
          }
          else if constexpr (requires { 
            { action.enPassantTile } -> std::same_as<TileCoords const &>; }) ///< if constexpr (std::is_same_v<T, ChessEnPassantItemMove>)
          {
            // remove en passant captured item
            auto enPassantItemEntry = gameRule_->getItemEntry(action.enPassantTile);
            BOOST_ASSERT_MSG(
                !enPassantItemEntry.isNull(),
                "There must be an item at the action.enPassantTile");

            itemStore_->removeItem(enPassantItemEntry);
          }
          else if constexpr (requires { 
                { action.rookSource } -> std::same_as<TileCoords const &>;
                { action.rookDestination } -> std::same_as<TileCoords const &>; }) ///< if constexpr (std::is_same_v<T, ChessCastlingItemMove>)
          {
            // do nothing
          }

          SPDLOG_INFO("You have finalized a '{}'", typeid(T).name());
        }
        else if constexpr (std::is_same_v<T, std::monostate>)
        {
          SPDLOG_WARN("You have finalized an empty move");
        }
        else if constexpr (std::is_same_v<T, InvalidChessItemMove>)
        {
          SPDLOG_WARN("You have finalized an 'InvalidChessItemMove'");
        }
      };

      pendingItemMoveAction_.visit(moveActionVisitor);
      gameRule_->commitMove(pendingItemMoveAction_);
      pendingItemMoveAction_.setEmpty();
    }

    SPDLOG_INFO("A message of type '{}' has been handled by '{}'",
                typeid(response).name(),
                "ChessPieceMovedState");
  }

  /////////////////////////////////////////////////////////////////////////

  void ChessPieceMovedState::onGameUpdatedNotification(
      GameUpdatedNotification const &notif) noexcept
  {
    auto moveActionVisitor = [this]<typename T>(T const &action)
    {
      if constexpr (requires {
          { action.fromTile } -> std::same_as<TileCoords const &>;
          { action.toTile } -> std::same_as<TileCoords const &>;
          { action.enemyKingTile } -> std::same_as<TileCoords const &>;
          { action.enemyKingState } -> std::same_as<KingState const &>; })
      {
        SPDLOG_INFO("Chess board has been updated with a '{}' from server",
                    typeid(T).name());

        auto movedItemEntry = gameRule_->getItemEntry(action.fromTile);
        BOOST_ASSERT_MSG(
            !movedItemEntry.isNull(),
            "There must be an item at the 'fromTile'");

        finalizeBasicMoveAction(action.fromTile, action.toTile);
        // fit the moved item to 'toTile'
        tileMap_->fitItemToTile(movedItemEntry.getItem(), action.toTile);

        if constexpr (requires { 
            { action.promote } -> std::same_as<std::string const &>; }) ///< if constexpr (std::is_same_v<T, ChessPromotionItemMove>)
        {
          auto itemColor = gameRule_->getItemColor(action.fromTile);
          Piece promotedPiece{action.promote, itemColor.value()};

          // change item of the moved item to the promoted one
          movedItemEntry.getItem() = itemStore_->createItem(
              int(utils::getChessTextureCellIndex(promotedPiece)),
              promotedPiece.toString());
        }
        else if constexpr (requires { 
            { action.enPassantTile } -> std::same_as<TileCoords const &>; }) ///< if constexpr (std::is_same_v<T, ChessEnPassantItemMove>)
        {
          // make en passant captured item invisible
          auto enPassantItemEntry = gameRule_->getItemEntry(action.enPassantTile);
          BOOST_ASSERT_MSG(
              !enPassantItemEntry.isNull(),
              "There must be an item at the action.enPassantTile");

          itemStore_->removeItem(enPassantItemEntry);
        }
        else if constexpr (requires { 
                { action.rookSource } -> std::same_as<TileCoords const &>;
                { action.rookDestination } -> std::same_as<TileCoords const &>; }) ///< if constexpr (std::is_same_v<T, ChessCastlingItemMove>)
        {
          // move the related rook to the destination
          auto rookItemEntry = gameRule_->getItemEntry(action.rookSource);
          BOOST_ASSERT_MSG(
              !rookItemEntry.isNull(),
              "There must be an item at the tile of action.rookSource");

          tileMap_->fitItemToTile(rookItemEntry.getItem(), action.rookDestination);
        }
      }
      else if constexpr (std::is_same_v<T, std::monostate>)
      {
        SPDLOG_WARN("An empty move from server has been throwed");
      }
      else if constexpr (std::is_same_v<T, InvalidChessItemMove>)
      {
        SPDLOG_WARN("An 'InvalidChessItemMove' from server has been throwed");
      }
    };

    auto enemyItemMoveAction = gameRule_->tryMove(ItemMove{
        gameRule_->positionToTile(notif.fromSquare),
        gameRule_->positionToTile(notif.toSquare),
        notif.promote});

    enemyItemMoveAction.visit(moveActionVisitor);
    gameRule_->commitMove(enemyItemMoveAction);

    SPDLOG_INFO("A message of type '{}' has been handled by '{}'",
                typeid(notif).name(),
                typeid(*this).name());
  }

  ///////////////////////////////////////////////////////////////////////

} // namespace bgg
