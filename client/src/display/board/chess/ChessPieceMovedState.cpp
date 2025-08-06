// ChessPieceMovedState.cpp

#include <boost/assert.hpp>

#include "ChessPieceMovedState.h"
#include "ChessPieceDisabledState.h"
#include "ChessTextureCell.h"
#include "IChessBoardView.h"
#include "IChessRuleAdapter.h"

#include "base/Logger.h"
#include "display/board/TileMap.h"
#include "display/board/ZOrder.h"
#include "service/ClientRequest.h"

namespace bgg
{
  /////////////////////////////////////////////////////////////////////////////
  ChessPieceMovedState::ChessPieceMovedState(
      std::shared_ptr<IChessBoardView> gameBoard,
      std::shared_ptr<IChessRuleAdapter> gameRule,
      std::shared_ptr<TileMap> tileMap,
      std::shared_ptr<ItemStore> itemStore,
      ChessItemMove const &itemMove) noexcept
      : gameBoard_(std::move(gameBoard)),
        gameRule_(std::move(gameRule)),
        tileMap_(std::move(tileMap)),
        itemStore_(std::move(itemStore)),
        pendingPromotionItem_{},
        pendingMoveHighlighters_{},
        opponentCheckHighlighter_{},
        lastMoveHighlighters_{nullptr, nullptr},
        lastMoveHighlighterVisibility_{false, false},
        allyCheckHighlighter_{nullptr},
        allyCheckHighlighterVisibility_(false),
        pendingMoveDetail_{std::nullopt}
  {
    try
    {
      pendingMoveDetail_ = gameRule_->tryMove(itemMove);
    }
    catch (std::exception const &e)
    {
      // if the move is invalid, replace the item to its original tile
      auto movedItemEntry = gameRule_->getItemEntry(itemMove.fromTile);
      BOOST_ASSERT_MSG(
          !movedItemEntry.isNull(), "There must be an item at the 'fromTile'");
      tileMap_->fitItemToTile(movedItemEntry.getItem(), itemMove.fromTile);
      SPDLOG_ERROR(
          "Invalid move from tile ({}, {}) to tile ({}, {}).\nError message: {}",
          itemMove.fromTile.x, itemMove.fromTile.y,
          itemMove.fromTile.x, itemMove.fromTile.y,
          e.what());
      return;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  void ChessPieceMovedState::onEnter(sf::Vector2i const & /*mousePos*/) noexcept
  {
    // SPDLOG_INFO("Entered '{}'", typeid(*this).name());

    if (pendingMoveDetail_)
    {
      TileCoords const &fromTile = pendingMoveDetail_->getSourceTile();
      TileCoords const &toTile = pendingMoveDetail_->getDestinationTile();
      auto promotedItemInfo = pendingMoveDetail_->getPromotedItemInfo();

      std::optional<EntityType> promotedPieceType{std::nullopt};
      if (promotedItemInfo)
      {
        promotedPieceType = promotedItemInfo->type;
      }

      ChessMove &&chessMove{
          gameRule_->getAllyColor(),
          gameRule_->tileToPosition(fromTile),
          gameRule_->tileToPosition(toTile),
          promotedPieceType};
      gameBoard_->sendMoveRequest(MoveRequest{"", "", chessMove});

      ///< initHighlighters() must be placed right here. No choice.
      initHighlighters();

      previewMoveAction();
    }
    else
    {
      gameBoard_->popState();
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  void ChessPieceMovedState::onServerMessage(ServerMessage const &msg) noexcept
  {
    if (auto moveResponse = msg.getIf<MoveResponse>())
    {
      if (moveResponse->errcode.failed())
      {
        revertMoveAction();
      }
      else
      {
        finalizeMoveAction();
      }
      SPDLOG_INFO("A message of type 'MoveResponse' has been handled by "
                  "'ChessPieceMovedState'");
    }
    else
    {
      SPDLOG_WARN(
          "The server message of type index {} is ignored by "
          "'ChessPieceDisabledState'",
          msg.getIndex());
    }
  }

  void ChessPieceMovedState::initHighlighters() noexcept
  {
    ///< find persistent highlighters of check
    std::list<BoardItem *> checkHighlighterList = itemStore_->findItemsIf(
        [](BoardItem const &item)
        {
          std::string const checkHighligherName = utils::toString(
              ChessTextureCell::CHECK_HIGHLIGHTER);
          return item.getName() == checkHighligherName;
        });

    BOOST_ASSERT_MSG(
        checkHighlighterList.size() == 1,
        "There must be exactly one checkHighlighter in the itemStore_");

    allyCheckHighlighter_ = checkHighlighterList.front();
    allyCheckHighlighterVisibility_ = allyCheckHighlighter_->isVisible();

    ///< find persistent highlighters of last move
    std::list<BoardItem *> lastMoveHighlighterList = itemStore_->findItemsIf(
        [](BoardItem const &item)
        {
          std::string const lastMoveHighligherName = utils::toString(
              ChessTextureCell::LAST_MOVE_HIGHLIGHTER);
          return item.getName() == lastMoveHighligherName;
        });

    BOOST_ASSERT_MSG(
        lastMoveHighlighterList.size() == 2,
        "There must be exactly two lastMoveHighlighters in the itemStore_");

    lastMoveHighlighters_[0] = lastMoveHighlighterList.front();
    lastMoveHighlighters_[1] = lastMoveHighlighterList.back();
    lastMoveHighlighterVisibility_[0] = lastMoveHighlighters_[0]->isVisible();
    lastMoveHighlighterVisibility_[1] = lastMoveHighlighters_[1]->isVisible();

    ///< add pending highlighters for the new move
    pendingMoveHighlighters_[0] = itemStore_->addItem(
        ZOrder::FIRST_LAYER,
        int(ChessTextureCell::LAST_MOVE_HIGHLIGHTER),
        "Pending" + utils::toString(ChessTextureCell::LAST_MOVE_HIGHLIGHTER),
        false);

    pendingMoveHighlighters_[1] = itemStore_->addItem(
        ZOrder::FIRST_LAYER,
        int(ChessTextureCell::LAST_MOVE_HIGHLIGHTER),
        "Pending" + utils::toString(ChessTextureCell::LAST_MOVE_HIGHLIGHTER),
        false);
  }

  /////////////////////////////////////////////////////////////////////////////
  void ChessPieceMovedState::previewMoveAction() noexcept
  {
    BOOST_ASSERT_MSG(
        pendingMoveDetail_,
        "An invalid or empty 'pendingMoveDetail_' does not make sense"
        "in this function");

    auto const &fromTile = pendingMoveDetail_->getSourceTile();
    auto const &toTile = pendingMoveDetail_->getDestinationTile();
    auto const &opponentKingTile = pendingMoveDetail_->getOpponentKingTile();
    auto const &opponentKingStatus = pendingMoveDetail_->getOpponentKingStatus();

    auto movedItemEntry = gameRule_->getItemEntry(fromTile);
    BOOST_ASSERT_MSG(
        !movedItemEntry.isNull(), "There must be an item at the 'fromTile'");

    ///< previewBasicMoveAction
    ///< 1. add highlighters for the new move
    lastMoveHighlighters_[0]->setVisible(false);
    lastMoveHighlighters_[1]->setVisible(false);
    tileMap_->fitItemToTile(pendingMoveHighlighters_[0].getItem(), fromTile);
    tileMap_->fitItemToTile(pendingMoveHighlighters_[1].getItem(), toTile);

    tileMap_->fitItemToTile(*movedItemEntry, toTile);

    ///< make captured item invisible
    auto capturedItemEntry = gameRule_->getItemEntry(toTile);
    if (!capturedItemEntry.isNull())
    {
      capturedItemEntry.getItem().setVisible(false);
    }

    allyCheckHighlighter_->setVisible(false); ///< always hide the check highlighter on your tile

    if (opponentKingStatus == Side::Status::IN_CHECK)
    {
      opponentCheckHighlighter_ = itemStore_->addItem(
          ZOrder::FIRST_LAYER,
          int(ChessTextureCell::CHECK_HIGHLIGHTER),
          utils::toString(ChessTextureCell::CHECK_HIGHLIGHTER));
      tileMap_->fitItemToTile(*opponentCheckHighlighter_, opponentKingTile);
    }
    else if (opponentKingStatus == Side::Status::CHECKMATED)
    {
      opponentCheckHighlighter_ = itemStore_->addItem(
          ZOrder::THIRD_LAYER,
          int(ChessTextureCell::CHECKMATE_HIGHLIGHTER),
          utils::toString(ChessTextureCell::CHECKMATE_HIGHLIGHTER));
      tileMap_->fitItemToTile(*opponentCheckHighlighter_, opponentKingTile);
    }

    ///< handle speciale moves
    if (auto promotedItemInfo = pendingMoveDetail_->getPromotedItemInfo())
    {
      ///< add new promoted item to itemStore_
      int textureCellIndex = int(utils::getChessTextureCellIndex(
          promotedItemInfo->type, promotedItemInfo->color));
      pendingPromotionItem_ = itemStore_->addItem(
          ZOrder::SECOND_LAYER,
          textureCellIndex,
          promotedItemInfo->toString());

      ///< fit the promoted item to 'toTile', make moved item invisible but not move it
      tileMap_->fitItemToTile(*pendingPromotionItem_, toTile);
      movedItemEntry.getItem().setVisible(false);
    }
    else if (auto enPassantCaptureTile =
                 pendingMoveDetail_->getEnPassantCaptureTile())
    {
      ///< make en passant captured item invisible
      auto enPassantItemEntry = gameRule_->getItemEntry(*enPassantCaptureTile);
      BOOST_ASSERT_MSG(
          !enPassantItemEntry.isNull(),
          "There must be an item at the enPassantCaptureTile");

      enPassantItemEntry.getItem().setVisible(false);
    }
    else if (auto rookMove = pendingMoveDetail_->getCastlingRookMove())
    {
      ///< move the related rook to the destination
      auto rookItemEntry = gameRule_->getItemEntry(rookMove->first);
      BOOST_ASSERT_MSG(
          !rookItemEntry.isNull(),
          "There must be an item at the tile of rookMove->first (rook source)");

      tileMap_->fitItemToTile(*rookItemEntry, rookMove->second);
    }
    // SPDLOG_INFO("You have tried to do a '{}'", typeid(T).name());
  }

  /////////////////////////////////////////////////////////////////////////////
  void ChessPieceMovedState::revertMoveAction() noexcept
  {
    BOOST_ASSERT_MSG(
        pendingMoveDetail_,
        "An invalid or empty 'pendingMoveDetail_' does not make sense"
        "in this function");

    auto const &fromTile = pendingMoveDetail_->getSourceTile();
    auto const &toTile = pendingMoveDetail_->getDestinationTile();
    // auto const &opponentKingTile = pendingMoveDetail_->getOpponentKingTile();
    // auto const &opponentKingStatus = pendingMoveDetail_->getOpponentKingStatus();

    auto movedItemEntry = gameRule_->getItemEntry(fromTile);
    BOOST_ASSERT_MSG(
        !movedItemEntry.isNull(),
        "There must be an item at the 'fromTile'");

    ///< restore lastMoveHighlighters_ to the previous tiles
    lastMoveHighlighters_[0]->setVisible(lastMoveHighlighterVisibility_[0]);
    lastMoveHighlighters_[1]->setVisible(lastMoveHighlighterVisibility_[1]);
    allyCheckHighlighter_->setVisible(allyCheckHighlighterVisibility_);

    itemStore_->removeItem(pendingMoveHighlighters_[0]);
    itemStore_->removeItem(pendingMoveHighlighters_[1]);
    itemStore_->removeItem(opponentCheckHighlighter_);

    ///< fit the moved item to 'fromTile'
    tileMap_->fitItemToTile(movedItemEntry.getItem(), fromTile);

    ///< make captured item visible
    auto capturedItemEntry = gameRule_->getItemEntry(toTile);
    if (!capturedItemEntry.isNull())
    {
      capturedItemEntry.getItem().setVisible(true);
    }

    ///< handle specific item move actions
    if (pendingMoveDetail_->getPromotedItemInfo())
    {
      ///< make the moved item visible and remove the pendingPromotionItem_
      movedItemEntry.getItem().setVisible(true);
      itemStore_->removeItem(pendingPromotionItem_);
    }
    else if (auto enPassantCaptureTile =
                 pendingMoveDetail_->getEnPassantCaptureTile())
    {
      ///< make en passant captured item visible
      auto enPassantItemEntry = gameRule_->getItemEntry(*enPassantCaptureTile);
      BOOST_ASSERT_MSG(
          !enPassantItemEntry.isNull(),
          "There must be an item at the enPassantCaptureTile");

      enPassantItemEntry.getItem().setVisible(true);
    }
    else if (auto rookMove = pendingMoveDetail_->getCastlingRookMove())
    {
      ///< move the related rook to the source
      auto rookItemEntry = gameRule_->getItemEntry(rookMove->first);
      BOOST_ASSERT_MSG(
          !rookItemEntry.isNull(),
          "There must be an item at the tile of rookMove->first (rook source)");

      tileMap_->fitItemToTile(rookItemEntry.getItem(), rookMove->first);
    }

    // SPDLOG_INFO(
    //     "You have reverted a move from {} to {}", fromTile, toTile);

    ///< revert to ChessPiece SelectableState
    gameBoard_->popState();
  }

  /////////////////////////////////////////////////////////////////////////////
  void ChessPieceMovedState::finalizeMoveAction() noexcept
  {
    BOOST_ASSERT_MSG(
        pendingMoveDetail_,
        "An invalid or empty 'pendingMoveDetail_' does not make sense"
        "in this function");

    auto const &fromTile = pendingMoveDetail_->getSourceTile();
    auto const &toTile = pendingMoveDetail_->getDestinationTile();
    // auto const &opponentKingTile = pendingMoveDetail_->getOpponentKingTile();
    auto const &opponentKingStatus = pendingMoveDetail_->getOpponentKingStatus();

    auto movedItemEntry = gameRule_->getItemEntry(fromTile);
    BOOST_ASSERT_MSG(
        !movedItemEntry.isNull(),
        "There must be an item at the 'fromTile'");

    ///< remove captured item from itemStore_
    auto capturedItemEntry = gameRule_->getItemEntry(toTile);
    if (!capturedItemEntry.isNull())
    {
      itemStore_->removeItem(capturedItemEntry);
    }

    ///< handle special cases
    if (auto promotedItemInfo = pendingMoveDetail_->getPromotedItemInfo())
    {
      ///< replace the moved item with the promoted item, then remove the pending
      ///< promoted item from itemStore
      movedItemEntry.getItem() = pendingPromotionItem_.getItem();
      itemStore_->removeItem(pendingPromotionItem_);
    }
    else if (auto enPassantCaptureTile =
                 pendingMoveDetail_->getEnPassantCaptureTile())
    {
      ///< remove en passant captured item
      auto enPassantItemEntry = gameRule_->getItemEntry(*enPassantCaptureTile);
      BOOST_ASSERT_MSG(
          !enPassantItemEntry.isNull(),
          "There must be an item at the enPassantCaptureTile");

      itemStore_->removeItem(enPassantItemEntry);
    }
    else if (auto rookMove = pendingMoveDetail_->getCastlingRookMove())
    {
      ///< do nothing
    }

    ///< update lastMoveHighlighters_ and allyCheckHighlighter_,
    ///< and remove the temporary pendingMoveHighlighters_ and opponentCheckHighlighter_
    tileMap_->fitItemToTile(*(lastMoveHighlighters_[0]), fromTile);
    tileMap_->fitItemToTile(*(lastMoveHighlighters_[1]), toTile);

    itemStore_->removeItem(pendingMoveHighlighters_[0]);
    itemStore_->removeItem(pendingMoveHighlighters_[1]);

    if (opponentKingStatus == Side::Status::IN_CHECK)
    {
      *allyCheckHighlighter_ = *opponentCheckHighlighter_;
      itemStore_->removeItem(opponentCheckHighlighter_);
    }
    else if (opponentKingStatus == Side::Status::CHECKMATED)
    {
      ///< handle the winning case
      ///< => do nothing, just wait for the server to send a GameFinishedNotification
    }

    gameRule_->commitMove(*pendingMoveDetail_);
    // itemStore_->removeItem(allyCheckHighlighter_);

    pendingMoveDetail_.reset();

    std::shared_ptr<IBoardViewState> &&
        pieceDisabledState = std::make_shared<ChessPieceDisabledState>(
            gameBoard_, gameRule_, tileMap_, itemStore_);
    gameBoard_->changeState(pieceDisabledState);
    // SPDLOG_INFO("You have finalized a '{}'", typeid(T).name());
  }
} // namespace bgg
