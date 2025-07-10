// SfBoardPieceSelectedState.cpp

#include <boost/assert.hpp>

#include "base/Logger.h"

#include "SfBoardPieceSelectedState.h"
#include "SfBoardPieceDisabledState.h"
#include "SfGameBoard.h"
#include "SfTileMap.h"

#include "ChessTextureCell.h"
#include "model/ChessPiece.h"
#include "ZOrder.h"
namespace bgg
{
  SfBoardPieceSelectedState::SfBoardPieceSelectedState(
      std::shared_ptr<SfGameBoard> gameBoard,
      std::shared_ptr<SfGameRuleAdapter> gameRule,
      std::shared_ptr<SfTileMap> tileMap,
      std::shared_ptr<SfItemStore> itemStore,
      TileCoords selectedTile) noexcept
      : gameBoard_(std::move(gameBoard)),
        gameRule_(std::move(gameRule)),
        tileMap_(std::move(tileMap)),
        itemStore_(std::move(itemStore)),
        choiceHighlighter_(*itemStore_),
        selectedItemEntry_(*itemStore_),
        originalSelectedItemZOrder_(-1),
        selectedTile_(std::move(selectedTile)),
        lastHoveredTile_{-1, -1}
  {
    choiceHighlighter_ = itemStore_->addItem(
        ChessTextureCell::CHOICE_HIGHLIGHTER,
        ZOrder::THIRD_LAYER,
        ChessTextureCell::toString(ChessTextureCell::CHOICE_HIGHLIGHTER).value(),
        false);

    bool somethingWrong = true;
    if (auto itemIndex = gameRule_->getItemIndex(selectedTile_))
    {
      if (auto itemEntryOpt = gameRule_->getItemEntry(itemIndex.value()))
      {
        selectedItemEntry_ = itemEntryOpt.value();
        originalSelectedItemZOrder_ = itemStore_->getZOrder(selectedItemEntry_);
        itemStore_->changeZOrder(selectedItemEntry_, ZOrder::FOURTH_LAYER);

        somethingWrong = somethingWrong && false;
      }
    }

    if (auto reachableTileInfo = gameRule_->getReachableTiles(selectedTile_))
    {
      addHighlighters(reachableTileInfo.value());
      somethingWrong = somethingWrong && false;

      //==========
      SPDLOG_DEBUG("There are {} reachable tiles",
                   reachableTileInfo.value().quietMoves.size() +
                       reachableTileInfo.value().captureMoves.size() +
                       reachableTileInfo.value().specialMoves.size());
      //==========
    }

    if (somethingWrong)
    {
      gameBoard_->popState();

      //==========
      SPDLOG_WARN(
          "Something wrong: there is no reachable tile from "
          "or no item at the selected tile ({}, {})",
          selectedTile_.x, selectedTile_.y);
      //==========
    }
  }

  SfBoardPieceSelectedState::~SfBoardPieceSelectedState()
  {
    itemStore_->removeItem(choiceHighlighter_);
    for (auto &highlighter : staticHighlighters_)
    {
      itemStore_->removeItem(highlighter);
    }
  }

  void SfBoardPieceSelectedState::onEnter() noexcept
  {
    //==========
    SPDLOG_INFO("Entered {}", typeid(*this).name());
    //==========
  }

  void SfBoardPieceSelectedState::onExit() noexcept
  {
    itemStore_->changeZOrder(selectedItemEntry_, originalSelectedItemZOrder_);
    choiceHighlighter_.getItem().setVisible(false);
    for (auto &highlighter : staticHighlighters_)
    {
      highlighter.getItem().setVisible(false);
    }
  }

  void SfBoardPieceSelectedState::onMouseMoved(sf::Vector2i const &mousePos) noexcept
  {
    SfBoardItem &selectedItem = selectedItemEntry_.getItem();
    selectedItem.setPosition(
        mousePos - selectedItem.getSize() / 2 + sf::Vector2i{1, 1});

    TileCoords tile = tileMap_->screenToTile(mousePos);
    if (tile == lastHoveredTile_)
    {
      return;
    }

    SfItemPlacementMap::iterator found = reachableTiles_.find(tile);
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

  void SfBoardPieceSelectedState::onMousePressed(
      sf::Vector2i const & /*mousePos*/) noexcept
  {
    // do nothing
  }

  void SfBoardPieceSelectedState::onMouseReleased(
      sf::Vector2i const &mousePos) noexcept
  {
    TileCoords tile = tileMap_->screenToTile(mousePos);
    SfItemPlacementMap::iterator found = reachableTiles_.find(tile);
    if (found == reachableTiles_.end())
    {
      gameBoard_->popState();
      return;
    }

    // choiceHighlighter_.getItem().setVisible(false);

    std::shared_ptr<SfBoardState>
        pieceDisabledState = std::make_shared<SfBoardPieceDisabledState>(
            gameBoard_, gameRule_, tileMap_, itemStore_);
    gameBoard_->changeState(pieceDisabledState);
  }

  void SfBoardPieceSelectedState::addHighlighters(
      SfReachableTileInfo &reachableTileInfo) noexcept
  {
    SfItemStore::Entry &&selectedTileHighlighter = itemStore_->addItem(
        ChessTextureCell::LAST_MOVE_HIGHLIGHTER,
        ZOrder::FIRST_LAYER,
        ChessTextureCell::toString(ChessTextureCell::LAST_MOVE_HIGHLIGHTER).value());

    tileMap_->fitItemToTile(selectedTileHighlighter.getItem(), selectedTile_);
    staticHighlighters_.emplace_back(selectedTileHighlighter);

    for (TileCoords &tile : reachableTileInfo.quietMoves)
    {
      SfItemStore::Entry quietMoveHighlighter = itemStore_->addItem(
          ChessTextureCell::QUIET_MOVE_HIGHLIGHTER,
          ZOrder::THIRD_LAYER,
          ChessTextureCell::toString(ChessTextureCell::QUIET_MOVE_HIGHLIGHTER).value());

      tileMap_->fitItemToTile(quietMoveHighlighter.getItem(), tile);
      staticHighlighters_.emplace_back(quietMoveHighlighter);

      auto [iter, inserted] = reachableTiles_.try_emplace(
          tile, std::move(quietMoveHighlighter));

      BOOST_ASSERT_MSG(inserted, "There should be one item per tile");
    }

    for (TileCoords &tile : reachableTileInfo.captureMoves)
    {
      SfItemStore::Entry &&captureMoveHighlighter = itemStore_->addItem(
          ChessTextureCell::CAPTURE_MOVE_HIGHLIGHTER,
          ZOrder::THIRD_LAYER,
          ChessTextureCell::toString(ChessTextureCell::CAPTURE_MOVE_HIGHLIGHTER).value());

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
    std::optional<int> pieceAtSpecialTile = gameRule_->getItemIndex(specialMoveTile);
    if (!pieceAtSpecialTile.has_value())
    {
      SfItemStore::Entry &&quietMoveHighlighter = itemStore_->addItem(
          ChessTextureCell::QUIET_MOVE_HIGHLIGHTER,
          ZOrder::THIRD_LAYER,
          ChessTextureCell::toString(ChessTextureCell::QUIET_MOVE_HIGHLIGHTER).value());

      tileMap_->fitItemToTile(quietMoveHighlighter.getItem(), specialMoveTile);
      staticHighlighters_.emplace_back(quietMoveHighlighter);

      auto [iter, inserted] = reachableTiles_.try_emplace(
          specialMoveTile, std::move(quietMoveHighlighter));

      BOOST_ASSERT_MSG(inserted, "There should be one item per tile");
    }
    else
    {
      SfItemStore::Entry &&captureMoveHighlighter = itemStore_->addItem(
          ChessTextureCell::CAPTURE_MOVE_HIGHLIGHTER,
          ZOrder::THIRD_LAYER,
          ChessTextureCell::toString(ChessTextureCell::CAPTURE_MOVE_HIGHLIGHTER).value());

      tileMap_->fitItemToTile(captureMoveHighlighter.getItem(), specialMoveTile);
      staticHighlighters_.emplace_back(captureMoveHighlighter);

      auto [iter, inserted] = reachableTiles_.try_emplace(
          specialMoveTile, std::move(captureMoveHighlighter));

      BOOST_ASSERT_MSG(inserted, "There should be one item per tile");
    }
  }
} // namespace bgg
