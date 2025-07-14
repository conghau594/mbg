// ChessPieceSelectedState.cpp

#include <boost/assert.hpp>

#include "base/Logger.h"

#include "ChessPieceSelectedState.h"
#include "IChessBoard.h"
#include "display/board/TileMap.h"
#include "display/board/ZOrder.h"

#include "ChessTextureCell.h"

#include "model/ChessPiece.h"
#include "service/ClientRequest.h"

namespace bgg
{
  ChessPieceSelectedState::ChessPieceSelectedState(
      std::shared_ptr<IChessBoard> gameBoard,
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
        originalSelectedItemZOrder_(-1),
        selectedTile_(std::move(selectedTile)),
        lastHoveredTile_{-1, -1}
  {
    choiceHighlighter_ = itemStore_->addItem(
        ChessTextureCell::CHOICE_HIGHLIGHTER,
        ZOrder::THIRD_LAYER,
        ChessTextureCell::toString(ChessTextureCell::CHOICE_HIGHLIGHTER),
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

      //==========
      SPDLOG_DEBUG(
          "There are {} reachable tiles from tile ({}, {}) obtained by '{}'",
          reachableTileInfo->quietMoves.size() +
              reachableTileInfo->captureMoves.size() +
              reachableTileInfo->specialMoves.size(),
          selectedTile_.x, selectedTile_.y,
          "?" /*ChessPiece::toString(gameRule_->getItemT Entry(selectedTile_).value())*/);
      //==========
    }

    BOOST_ASSERT_MSG(!somethingWrong,
                     "Something wrong: there is no reachable tile from "
                     "or no item at the selectedTile_");
  }

  ChessPieceSelectedState::~ChessPieceSelectedState()
  {
    itemStore_->removeItem(choiceHighlighter_);
    for (auto &highlighter : staticHighlighters_)
    {
      itemStore_->removeItem(highlighter);
    }
  }

  void ChessPieceSelectedState::onEnter(sf::Vector2i const &mousePos) noexcept
  {
    onMouseMoved(mousePos);

    //==========
    SPDLOG_INFO("Entered '{}'", typeid(*this).name());
    //==========
  }

  void ChessPieceSelectedState::onExit() noexcept
  {
    itemStore_->changeZOrder(selectedItemEntry_, originalSelectedItemZOrder_);
    choiceHighlighter_.getItem().setVisible(false);
    for (auto &highlighter : staticHighlighters_)
    {
      highlighter.getItem().setVisible(false);
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
    }
    else
    {
      tileMap_->fitItemToTile(choiceHighlighter_.getItem(), tile);
      choiceHighlighter_.getItem().setVisible(true);
    }

    lastHoveredTile_ = tile;
  }

  void ChessPieceSelectedState::onMousePressed(
      sf::Vector2i const & /*mousePos*/) noexcept
  {
    // do nothing
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

    gameBoard_->clearStates();
    gameBoard_->requestMove(ChessPieceMove{
        selectedTile_, targetedTile, std::nullopt});
  }

  void ChessPieceSelectedState::addHighlighters(
      ReachableTileInfo &reachableTileInfo) noexcept
  {
    ItemStore::Entry &&selectedTileHighlighter = itemStore_->addItem(
        ChessTextureCell::LAST_MOVE_HIGHLIGHTER,
        ZOrder::FIRST_LAYER,
        ChessTextureCell::toString(ChessTextureCell::LAST_MOVE_HIGHLIGHTER));

    tileMap_->fitItemToTile(selectedTileHighlighter.getItem(), selectedTile_);
    staticHighlighters_.emplace_back(selectedTileHighlighter);

    for (TileCoords &tile : reachableTileInfo.quietMoves)
    {
      ItemStore::Entry quietMoveHighlighter = itemStore_->addItem(
          ChessTextureCell::QUIET_MOVE_HIGHLIGHTER,
          ZOrder::THIRD_LAYER,
          ChessTextureCell::toString(ChessTextureCell::QUIET_MOVE_HIGHLIGHTER));

      tileMap_->fitItemToTile(quietMoveHighlighter.getItem(), tile);
      staticHighlighters_.emplace_back(quietMoveHighlighter);

      auto [iter, inserted] = reachableTiles_.try_emplace(
          tile, std::move(quietMoveHighlighter));

      BOOST_ASSERT_MSG(inserted, "There should be one item per tile");
    }

    for (TileCoords &tile : reachableTileInfo.captureMoves)
    {
      ItemStore::Entry &&captureMoveHighlighter = itemStore_->addItem(
          ChessTextureCell::CAPTURE_MOVE_HIGHLIGHTER,
          ZOrder::THIRD_LAYER,
          ChessTextureCell::toString(ChessTextureCell::CAPTURE_MOVE_HIGHLIGHTER));

      tileMap_->fitItemToTile(captureMoveHighlighter.getItem(), tile);
      staticHighlighters_.emplace_back(captureMoveHighlighter);

      auto [iter, inserted] = reachableTiles_.try_emplace(
          tile, std::move(captureMoveHighlighter));

      BOOST_ASSERT_MSG(inserted, "There should be one item per tile");
    }

    if (reachableTileInfo.specialMoves.empty())
    {
      return;
    }

    // TODO: IMPORTANT!!! The following snippet is specific to the Chess game.
    //  So this class cannot be common to other types of board game.
    //  You need a refactor. You might delegate this to gameRule_?
    TileCoords const &specialMoveTile = reachableTileInfo.specialMoves[0];
    auto itemAtSpecialTile = gameRule_->getItemEntry(specialMoveTile);
    if (!itemAtSpecialTile.isNull())
    {
      ItemStore::Entry &&captureMoveHighlighter = itemStore_->addItem(
          ChessTextureCell::CAPTURE_MOVE_HIGHLIGHTER,
          ZOrder::THIRD_LAYER,
          ChessTextureCell::toString(ChessTextureCell::CAPTURE_MOVE_HIGHLIGHTER));

      tileMap_->fitItemToTile(captureMoveHighlighter.getItem(), specialMoveTile);
      staticHighlighters_.emplace_back(captureMoveHighlighter);

      auto [iter, inserted] = reachableTiles_.try_emplace(
          specialMoveTile, std::move(captureMoveHighlighter));

      BOOST_ASSERT_MSG(inserted, "There should be one item per tile");
    }
    else
    {
      ItemStore::Entry &&quietMoveHighlighter = itemStore_->addItem(
          ChessTextureCell::QUIET_MOVE_HIGHLIGHTER,
          ZOrder::THIRD_LAYER,
          ChessTextureCell::toString(ChessTextureCell::QUIET_MOVE_HIGHLIGHTER));

      tileMap_->fitItemToTile(quietMoveHighlighter.getItem(), specialMoveTile);
      staticHighlighters_.emplace_back(quietMoveHighlighter);

      auto [iter, inserted] = reachableTiles_.try_emplace(
          specialMoveTile, std::move(quietMoveHighlighter));

      BOOST_ASSERT_MSG(inserted, "There should be one item per tile");
    }
  }
} // namespace bgg
