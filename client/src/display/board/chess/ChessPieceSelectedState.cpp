// ChessPieceSelectedState.cpp

#include <boost/assert.hpp>

#include "base/Logger.h"

#include "ChessPieceSelectedState.h"
#include "ChessPieceMovedState.h"
#include "IChessBoardView.h"
#include "display/board/TileMap.h"
#include "display/board/ZOrder.h"

#include "ChessTextureCell.h"

#include "service/ClientRequest.h"

namespace bgg
{
  ChessPieceSelectedState::ChessPieceSelectedState(
      std::shared_ptr<IChessBoardView> gameBoard,
      std::shared_ptr<IChessRuleAdapter> gameRule,
      std::shared_ptr<TileMap> tileMap,
      std::shared_ptr<ItemStore> itemStore,
      TileCoords selectedTile) noexcept
      : gameBoard_(std::move(gameBoard)),
        gameRule_(std::move(gameRule)),
        tileMap_(std::move(tileMap)),
        itemStore_(std::move(itemStore)),
        reachableTiles_(),
        staticHighlighters_(),
        choiceHighlighter_(),
        selectedItemEntry_(),
        enPassantCaptureHighlighter_(),
        originalSelectedItemZOrder_(-1),
        selectedTile_(std::move(selectedTile)),
        enPassantTile_{-1, -1},
        lastHoveredTile_{-1, -1}
  {
    choiceHighlighter_ = itemStore_->addItem(
        ZOrder::THIRD_LAYER,
        int(ChessTextureCell::CHOICE_HIGHLIGHTER),
        utils::toString(ChessTextureCell::CHOICE_HIGHLIGHTER),
        false);

    bool somethingWrong = true;
    auto itemEntry = gameRule_->getItemEntry(selectedTile_);
    if (!itemEntry.isNull())
    {
      selectedItemEntry_ = itemEntry;
      originalSelectedItemZOrder_ = itemStore_->getZOrder(selectedItemEntry_);
      itemStore_->changeZOrder(selectedItemEntry_, ZOrder::FOURTH_LAYER);

      somethingWrong = false;
    }

    if (auto reachableTileInfo = gameRule_->getReachableTiles(selectedTile_))
    {
      addHighlighters(reachableTileInfo.value());
      somethingWrong = false;

      // SPDLOG_DEBUG(
      //     "There are {} reachable tiles from tile ({}, {}) obtained by '{}'",
      //     reachableTileInfo->quietSquares.size() +
      //         reachableTileInfo->captureSquares.size() +
      //         reachableTileInfo->specialMoveTiles.size(),
      //     selectedTile_.x, selectedTile_.y,
      //     "?" /*ChessPiece::toString(gameRule_->getItemT Entry(selectedTile_).value())*/);
    }

    BOOST_ASSERT_MSG(!somethingWrong,
                     "Something wrong: there is no reachable tile from "
                     "or no item at the selectedTile_");
  }

  void ChessPieceSelectedState::onEnter(sf::Vector2i const &mousePos) noexcept
  {
    onMouseMoved(mousePos);

    // SPDLOG_INFO("Entered '{}'", typeid(*this).name());
  }

  void ChessPieceSelectedState::onExit() noexcept
  {
    itemStore_->changeZOrder(selectedItemEntry_, originalSelectedItemZOrder_);

    itemStore_->removeItem(choiceHighlighter_);
    itemStore_->removeItem(enPassantCaptureHighlighter_);
    for (auto &highlighter : staticHighlighters_)
    {
      itemStore_->removeItem(highlighter);
    }

    lastHoveredTile_ = {-1, -1};
  }

  void ChessPieceSelectedState::onMouseMoved(sf::Vector2i const &mousePos) noexcept
  {
    BoardItem &selectedItem = selectedItemEntry_.getItem();
    selectedItem.setPosition(mousePos - selectedItem.getSize() / 2);

    TileCoords tile = tileMap_->screenToTile(mousePos);
    if (tile == lastHoveredTile_)
    {
      return;
    }

    ItemPlacementMap::iterator found = reachableTiles_.find(tile);
    if (found == reachableTiles_.end())
    {
      choiceHighlighter_.getItem().setVisible(false);
      if (!enPassantCaptureHighlighter_.isNull())
      {
        enPassantCaptureHighlighter_.getItem().setVisible(false);
      }
    }
    else
    {
      tileMap_->fitItemToTile(choiceHighlighter_.getItem(), tile);
      choiceHighlighter_.getItem().setVisible(true);
      if (!enPassantCaptureHighlighter_.isNull())
      {
        if (enPassantTile_ == tile + sf::Vector2i{0, 1})
        {
          enPassantCaptureHighlighter_.getItem().setVisible(true);
        }
        else
        {
          enPassantCaptureHighlighter_.getItem().setVisible(false);
        }
      }
    }

    lastHoveredTile_ = tile;
  }

  void ChessPieceSelectedState::onMousePressed(
      sf::Vector2i const & /*mousePos*/) noexcept
  {
    ///< do nothing
  }

  void ChessPieceSelectedState::onMouseReleased(
      sf::Vector2i const &mousePos) noexcept
  {
    TileCoords targetedTile = tileMap_->screenToTile(mousePos);
    ItemPlacementMap::iterator found = reachableTiles_.find(targetedTile);
    if (found == reachableTiles_.end())
    {
      tileMap_->fitItemToTile(selectedItemEntry_.getItem(), selectedTile_);
      gameBoard_->popState(mousePos);
      return;
    }
    // choiceHighlighter_.getItem().setVisible(false);

    std::optional<std::string> promotedPiece(std::nullopt);
    if (targetedTile.y == 0 &&
        gameRule_->getItemType(selectedTile_) == ChessRule::PAWN)
    {
      // TODO: make 'promotedPiece' choosable instead fixed "Queen" like this
      promotedPiece = "Queen";
    }

    ChessItemMove move{selectedTile_, targetedTile, promotedPiece};
    std::shared_ptr<IBoardViewState>
        pieceMovedState = std::make_shared<ChessPieceMovedState>(
            gameBoard_, gameRule_, tileMap_, itemStore_, std::move(move));

    gameBoard_->changeState(std::move(pieceMovedState));
  }

  void ChessPieceSelectedState::onServerMessage(
      ServerMessage const & /*msg*/) noexcept
  {
    ///< do nothing
  }

  void ChessPieceSelectedState::addHighlighters(
      ReachableTileInfo &reachableTileInfo) noexcept
  {
    ItemStore::Entry selectedTileHighlighter = itemStore_->addItem(
        ZOrder::FIRST_LAYER,
        int(ChessTextureCell::LAST_MOVE_HIGHLIGHTER),
        utils::toString(ChessTextureCell::LAST_MOVE_HIGHLIGHTER));

    tileMap_->fitItemToTile(selectedTileHighlighter.getItem(), selectedTile_);
    staticHighlighters_.emplace_back(std::move(selectedTileHighlighter));

    for (TileCoords &tile : reachableTileInfo.quietSquares)
    {
      ItemStore::Entry quietMoveHighlighter = itemStore_->addItem(
          ZOrder::THIRD_LAYER,
          int(ChessTextureCell::QUIET_MOVE_HIGHLIGHTER),
          utils::toString(ChessTextureCell::QUIET_MOVE_HIGHLIGHTER));

      tileMap_->fitItemToTile(quietMoveHighlighter.getItem(), tile);
      staticHighlighters_.emplace_back(quietMoveHighlighter);

      auto [iter, inserted] = reachableTiles_.try_emplace(
          tile, std::move(quietMoveHighlighter));

      BOOST_ASSERT_MSG(inserted, "There should be one item per tile");
    }

    for (TileCoords &tile : reachableTileInfo.captureSquares)
    {
      ItemStore::Entry captureMoveHighlighter = itemStore_->addItem(
          ZOrder::THIRD_LAYER,
          int(ChessTextureCell::CAPTURE_MOVE_HIGHLIGHTER),
          utils::toString(ChessTextureCell::CAPTURE_MOVE_HIGHLIGHTER));

      tileMap_->fitItemToTile(captureMoveHighlighter.getItem(), tile);
      staticHighlighters_.emplace_back(captureMoveHighlighter);

      auto [iter, inserted] = reachableTiles_.try_emplace(
          tile, std::move(captureMoveHighlighter));

      BOOST_ASSERT_MSG(inserted, "There should be one item per tile");
    }

    if (reachableTileInfo.specialMoveTiles.empty())
    {
      return;
    }

    // handle special moves

    if (reachableTileInfo.specialMoveTiles.size() == 1)
    {
      // handle en passant capture
      enPassantTile_ = reachableTileInfo.specialMoveTiles.front();
      BOOST_ASSERT_MSG(
          !gameRule_->getItemEntry(enPassantTile_).isNull(),
          "There should be one item at the en passant tile");

      ItemStore::Entry quietMoveHighlighter = itemStore_->addItem(
          ZOrder::THIRD_LAYER,
          int(ChessTextureCell::QUIET_MOVE_HIGHLIGHTER),
          utils::toString(ChessTextureCell::QUIET_MOVE_HIGHLIGHTER));
      TileCoords const reachableTile = enPassantTile_ - sf::Vector2i{0, 1};
      tileMap_->fitItemToTile(quietMoveHighlighter.getItem(), reachableTile);
      staticHighlighters_.emplace_back(quietMoveHighlighter);

      auto [iter, inserted] = reachableTiles_.try_emplace(
          reachableTile, std::move(quietMoveHighlighter));

      enPassantCaptureHighlighter_ = itemStore_->addItem(
          ZOrder::THIRD_LAYER,
          int(ChessTextureCell::CAPTURE_MOVE_HIGHLIGHTER),
          utils::toString(ChessTextureCell::CAPTURE_MOVE_HIGHLIGHTER));
      tileMap_->fitItemToTile(enPassantCaptureHighlighter_.getItem(), enPassantTile_);
      enPassantCaptureHighlighter_.getItem().setVisible(false);
      // staticHighlighters_.emplace_back(enPassantCaptureHighlighter_);

      BOOST_ASSERT_MSG(inserted, "There should be one item per tile");
    }
    else
    {
      ///< handle castling move
      for (auto &tile : reachableTileInfo.specialMoveTiles)
      {
        ItemStore::Entry quietMoveHighlighter = itemStore_->addItem(
            ZOrder::THIRD_LAYER,
            int(ChessTextureCell::QUIET_MOVE_HIGHLIGHTER),
            utils::toString(ChessTextureCell::QUIET_MOVE_HIGHLIGHTER));

        tileMap_->fitItemToTile(quietMoveHighlighter.getItem(), tile);
        staticHighlighters_.emplace_back(quietMoveHighlighter);

        auto [iter, inserted] = reachableTiles_.try_emplace(
            tile, std::move(quietMoveHighlighter));

        BOOST_ASSERT_MSG(inserted, "There should be one item per tile");
      }
    }
  }
} // namespace bgg
