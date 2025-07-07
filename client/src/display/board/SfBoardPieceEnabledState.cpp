// SfBoardPieceEnabledState.cpp

#include "SfBoardPieceEnabledState.h"
#include "SfBoardPieceSelectedState.h"
#include "SfGameBoard.h"
#include "ChessTextureCell.h"
#include "ZOrder.h"
#include "SfTileMap.h"

#ifdef _DEBUG
#include <iostream>
#include <format>
#endif
namespace bgg
{
  SfBoardPieceEnabledState::SfBoardPieceEnabledState(
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
  }

  void SfBoardPieceEnabledState::onEnter() noexcept
  {
    selectableTiles_ = gameRule_->getSelectableTiles();

#ifdef _DEBUG
    std::string debugInfo = std::format(
        "\nThere are {} selectable tiles.",
        selectableTiles_.size());
    std::clog << debugInfo;
#endif
  }

  void SfBoardPieceEnabledState::onExit() noexcept
  {
    itemStore_->removeItem(choiceHighlighter_);
  }

  void SfBoardPieceEnabledState::onMouseMoved(sf::Vector2i const &mousePos) noexcept
  {
    TileCoords tile = tileMap_->screenToTile(mousePos);
    if (tile == lastHoveredTile_)
    {
      return;
    }

    SfItemPlacementMap::iterator found = selectableTiles_.find(tile);
    if (found != selectableTiles_.end())
    {
      tileMap_->fitItemToTile(choiceHighlighter_.getItem(), tile);
      choiceHighlighter_.getItem().setVisible(true);
    }
    else
    {
      choiceHighlighter_.getItem().setVisible(false);
    }

    lastHoveredTile_ = tile;
  }

  void SfBoardPieceEnabledState::onMousePressed(sf::Vector2i const &mousePos) noexcept
  {
    // TODO:

    std::shared_ptr<SfBoardState>
        pieceSelectedState = std::make_shared<SfBoardPieceSelectedState>(
            gameBoard_, gameRule_, tileMap_, itemStore_);
    gameBoard_->changeState(pieceSelectedState);
  }

  void SfBoardPieceEnabledState::onMouseReleased(sf::Vector2i const &mousePos) noexcept
  {
    // do nothing
  }
} // namespace bgg
