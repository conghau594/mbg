// SfBoardPieceSelectedState.cpp

#include <boost/assert.hpp>

#include "base/Logger.h"

#include "SfBoardPieceSelectedState.h"
#include "SfBoardPieceEnabledState.h"
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
        selectedTile_(std::move(selectedTile)),
        lastHoveredTile_{-1, -1}
  {
  }

  void SfBoardPieceSelectedState::onEnter() noexcept
  {
    //==========
    SPDLOG_INFO("Entered {}", typeid(*this).name());
    //==========

    choiceHighlighter_ = itemStore_->addItem(
        ChessTextureCell::CHOICE_HIGHLIGHTER,
        ZOrder::THIRD_LAYER,
        ChessTextureCell::toString(ChessTextureCell::CHOICE_HIGHLIGHTER).value(),
        false);

    std::optional<SfReachableTileInfo>
        reachableTileInfo = gameRule_->getReachableTiles(selectedTile_);
    if (!reachableTileInfo.has_value())
    {
      std::shared_ptr<SfBoardState> pieceEnabledState =
          std::make_shared<SfBoardPieceEnabledState>(
              gameBoard_, gameRule_, tileMap_, itemStore_);

      gameBoard_->changeState(pieceEnabledState);

      //==========
      SPDLOG_WARN("There is no reachable tile");
      //==========
      return;
    }

    addHighlighters(reachableTileInfo.value());

    //==========
    SPDLOG_DEBUG("There are {} reachable tiles",
                 reachableTileInfo.value().quietMoves.size() +
                     reachableTileInfo.value().captureMoves.size() +
                     reachableTileInfo.value().specialMoves.size());
    //==========
  }

  void SfBoardPieceSelectedState::onExit() noexcept
  {
    itemStore_->removeItem(choiceHighlighter_);
    for (auto &highlighter : staticHighlighters_)
    {
      itemStore_->removeItem(highlighter);
    }
  }

  void SfBoardPieceSelectedState::onMouseMoved(sf::Vector2i const &mousePos) noexcept
  {
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
      std::shared_ptr<SfBoardState> pieceEnabledState =
          std::make_shared<SfBoardPieceEnabledState>(
              gameBoard_, gameRule_, tileMap_, itemStore_);

      gameBoard_->changeState(pieceEnabledState);
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
