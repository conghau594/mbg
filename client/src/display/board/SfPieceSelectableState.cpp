// SfPieceSelectableState.cpp

#include "base/Logger.h"

#include "SfPieceSelectableState.h"
#include "SfPieceSelectedState.h"
#include "SfGameBoard.h"
#include "SfTileMap.h"

#include "ChessTextureCell.h"
#include "ZOrder.h"
#include "model/ChessPiece.h"

namespace bgg
{
  SfPieceSelectableState::SfPieceSelectableState(
      std::shared_ptr<SfGameBoard> gameBoard,
      std::shared_ptr<SfGameRuleAdapter> gameRule,
      std::shared_ptr<SfTileMap> tileMap,
      std::shared_ptr<SfItemStore> itemStore) noexcept
      : gameBoard_(std::move(gameBoard)),
        gameRule_(std::move(gameRule)),
        tileMap_(std::move(tileMap)),
        itemStore_(std::move(itemStore)),
        choiceHighlighter_(*itemStore_),
        lastHoveredTile_{-1, -1}
  {
    choiceHighlighter_ = itemStore_->addItem(
        ChessTextureCell::CHOICE_HIGHLIGHTER,
        ZOrder::THIRD_LAYER,
        ChessTextureCell::toString(ChessTextureCell::CHOICE_HIGHLIGHTER).value(),
        false);

    selectableTiles_ = gameRule_->getSelectableTiles();
    //==========
    SPDLOG_DEBUG("There are {} selectable tiles from '{}' side",
                 selectableTiles_.size(),
                 ChessColor::toString(gameRule_->getSide()).value());
    //==========
  }

  SfPieceSelectableState::~SfPieceSelectableState() noexcept
  {
    itemStore_->removeItem(choiceHighlighter_);
  }

  void SfPieceSelectableState::onEnter(sf::Vector2i const &mousePos) noexcept
  {
    onMouseMoved(mousePos);

    //==========
    SPDLOG_INFO("Entered '{}'", typeid(*this).name());
    //==========
  }

  void SfPieceSelectableState::onExit() noexcept
  {
    choiceHighlighter_.getItem().setVisible(false);
    lastHoveredTile_ = {-1, -1};
  }

  void SfPieceSelectableState::onMouseMoved(sf::Vector2i const &mousePos) noexcept
  {
    TileCoords tile = tileMap_->screenToTile(mousePos);
    if (tile == lastHoveredTile_)
    {
      return;
    }

    SfItemPlacementMap::iterator found = selectableTiles_.find(tile);
    if (found == selectableTiles_.end())
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

  void SfPieceSelectableState::onMousePressed(sf::Vector2i const &mousePos) noexcept
  {
    TileCoords targetedTile = tileMap_->screenToTile(mousePos);
    SfItemPlacementMap::iterator found = selectableTiles_.find(targetedTile);
    if (found == selectableTiles_.end())
    {
      return;
    }

    std::shared_ptr<SfBoardState>
        pieceSelectedState = std::make_shared<SfPieceSelectedState>(
            gameBoard_, gameRule_, tileMap_, itemStore_, targetedTile);
    gameBoard_->pushState(pieceSelectedState, mousePos);
  }

  void SfPieceSelectableState::onMouseReleased(sf::Vector2i const & /*mousePos*/) noexcept
  {
    // do nothing
  }
} // namespace bgg
