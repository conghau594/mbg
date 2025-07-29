// ChessPieceDisabledState.cpp

#include <boost/assert.hpp>

#include "ChessPieceDisabledState.h"

#include "ChessTextureCell.h"
#include "IChessBoardView.h"
#include "IChessRuleAdapter.h"

#include "display/board/TileMap.h"
#include "display/board/ZOrder.h"
#include "base/Logger.h"

namespace bgg
{
  ChessPieceDisabledState::ChessPieceDisabledState(
      std::shared_ptr<IChessBoardView> gameBoard,
      std::shared_ptr<IChessRuleAdapter> gameRule,
      std::shared_ptr<TileMap> tileMap,
      std::shared_ptr<ItemStore> itemStore) noexcept
      : gameBoard_(std::move(gameBoard)),
        gameRule_(std::move(gameRule)),
        tileMap_(std::move(tileMap)),
        itemStore_(std::move(itemStore)),
        lastMoveHighlighters_{nullptr, nullptr},
        checkHighlighter_{nullptr}
  {

    ///< find persistent highlighters of check
    std::list<BoardItem *> checkHighlighterList = itemStore_->findItems(
        [](BoardItem const &item)
        {
          std::string const checkHighligherName = utils::toString(
              ChessTextureCell::CHECK_HIGHLIGHTER);
          return item.getName() == checkHighligherName;
        });

    BOOST_ASSERT_MSG(
        checkHighlighterList.size() == 1,
        "There must be exactly one checkHighlighter in the itemStore_");

    checkHighlighter_ = checkHighlighterList.front();
    // checkHighlighterVisibility_ = allyCheckHighlighter_->isVisible();

    ///< find persistent highlighters of last move
    std::list<BoardItem *> lastMoveHighlighterList = itemStore_->findItems(
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
    // lastMoveHighlighterVisibility_[0] = lastMoveHighlighters_[0]->isVisible();
    // lastMoveHighlighterVisibility_[1] = lastMoveHighlighters_[1]->isVisible();
  }

  void ChessPieceDisabledState::onServerMessage(ServerMessage const &msg) noexcept
  {
    if (auto gameUpdatedNotif = msg.getIf<GameUpdatedNotification>())
    {
      updatedOpponentMove(gameUpdatedNotif->opponentMoveDetail);
      SPDLOG_INFO("A message of type 'GameUpdatedNotification' has been "
                  "handled by 'ChessPieceMovedState'");
    }
    else
    {
      SPDLOG_WARN(
          "The server message of type index {} is ignored by "
          "'ChessPieceDisabledState'",
          msg.getIndex());
    }
  }

  /////////////////////////////////////////////////////////////////////////
  void ChessPieceDisabledState::updatedOpponentMove(
      ChessMove::Detail const &nativeMoveDetail) noexcept
  {
    ChessItemMove::Detail opponentItemMoveDetail{
        nativeMoveDetail,
        [this](Position const &pos) -> TileCoords
        {
          return gameRule_->positionToTile(pos);
        }};

    if (opponentItemMoveDetail.isEmpty() || !opponentItemMoveDetail.isValid())
    {
      SPDLOG_ERROR(
          "An invalid or empty 'opponentMoveDetail' in 'GameUpdatedNotification' "
          "has come to 'ChessPieceDisabledState' from server");
      return;
    }

    auto const &opponentFromTile = opponentItemMoveDetail.getSourceTile();
    auto const &opponentToTile = opponentItemMoveDetail.getDestinationTile();
    auto movedItemEntry = gameRule_->getItemEntry(opponentFromTile);
    BOOST_ASSERT_MSG(
        !movedItemEntry.isNull(),
        "There must be an item at the 'fromTile'");

    ///< remove captured item from itemStore_
    auto capturedItemEntry = gameRule_->getItemEntry(opponentToTile);
    if (!capturedItemEntry.isNull())
    {
      itemStore_->removeItem(capturedItemEntry);
    }

    ///< fit the moved item to 'toTile'
    tileMap_->fitItemToTile(movedItemEntry.getItem(), opponentToTile);

    ///< handle speciale moves
    if (auto promotedItemInfo = opponentItemMoveDetail.getPromotedItemInfo())
    {
      int textureCellIndex = int(utils::getChessTextureCellIndex(
          promotedItemInfo->type, promotedItemInfo->color));

      ///< change item of the moved item to the promoted one
      movedItemEntry.getItem() = itemStore_->createItem(
          textureCellIndex,
          promotedItemInfo->toString());
    }
    else if (auto enPassantCaptureTile =
                 opponentItemMoveDetail.getEnPassantCaptureTile())
    {
      // make en passant captured item invisible
      auto enPassantItemEntry = gameRule_->getItemEntry(*enPassantCaptureTile);
      BOOST_ASSERT_MSG(
          !enPassantItemEntry.isNull(),
          "There must be an item at the enPassantCaptureTile");

      itemStore_->removeItem(enPassantItemEntry);
    }
    else if (auto rookMove = opponentItemMoveDetail.getCastlingRookMove())
    {
      // move the castling rook item to the destination
      auto rookItemEntry = gameRule_->getItemEntry(rookMove->first);
      BOOST_ASSERT_MSG(
          !rookItemEntry.isNull(),
          "There must be an item at the tile of rookMove->first (rook source)");

      tileMap_->fitItemToTile(*rookItemEntry, rookMove->second);
    }

    ///< fit lastMoveHighlighters_ to fromTile and toTile
    tileMap_->fitItemToTile(*(lastMoveHighlighters_[0]), opponentFromTile);
    tileMap_->fitItemToTile(*(lastMoveHighlighters_[1]), opponentToTile);

    ///< handle check or checkmate of ally king
    auto const &allyKingTile = opponentItemMoveDetail.getOpponentKingTile();
    auto const &allyKingStatus = opponentItemMoveDetail.getOpponentKingStatus();

    if (allyKingStatus == Side::Status::IN_CHECK)
    {
      tileMap_->fitItemToTile(*checkHighlighter_, allyKingTile);
      gameBoard_->popState();
    }
    else if (allyKingStatus == Side::Status::SAFE)
    {
      gameBoard_->popState();
    }
    else // if (allyKingStatus == Side::Status::CHECKMATED)
    {
      ItemStore::Entry checkmateHighlighter = itemStore_->addItem(
          ZOrder::THIRD_LAYER,
          int(ChessTextureCell::CHECKMATE_HIGHLIGHTER),
          utils::toString(ChessTextureCell::CHECKMATE_HIGHLIGHTER));

      tileMap_->fitItemToTile(*checkmateHighlighter, allyKingTile);

      ///< handle the losing case
      ///< => do nothing, just wait for the server to send a GameFinishedNotification
    }

    gameRule_->commitMove(nativeMoveDetail);
  }

  ///////////////////////////////////////////////////////////////////////

} // namespace bgg
