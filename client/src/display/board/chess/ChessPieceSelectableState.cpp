// ChessPieceSelectableState.cpp

#include "base/Logger.h"

#include "ChessPieceSelectableState.h"
#include "ChessPieceSelectedState.h"

#include "IChessBoardView.h"
#include "display/board/TileMap.h"
#include "display/board/ZOrder.h"

#include "ChessTextureCell.h"

namespace bgg
{
  ChessPieceSelectableState::ChessPieceSelectableState(
      std::shared_ptr<IChessBoardView> gameBoard,
      std::shared_ptr<IChessRuleAdapter> gameRule,
      std::shared_ptr<TileMap> tileMap,
      std::shared_ptr<ItemStore> itemStore) noexcept
      : gameBoard_(std::move(gameBoard)),
        gameRule_(std::move(gameRule)),
        tileMap_(std::move(tileMap)),
        itemStore_(std::move(itemStore)),
        choiceHighlighter_(),
        lastHoveredTile_{-1, -1}
  {
    choiceHighlighter_ = itemStore_->addItem(
        ZOrder::THIRD_LAYER,
        int(ChessTextureCell::CHOICE_HIGHLIGHTER),
        utils::toString(ChessTextureCell::CHOICE_HIGHLIGHTER),
        false);

    SPDLOG_DEBUG("There are {} selectable tiles from '{}'",
                 selectableTiles_.size(), gameRule_->getAllyColor());
  }

  ChessPieceSelectableState::~ChessPieceSelectableState() noexcept
  {
    itemStore_->removeItem(choiceHighlighter_);
  }

  void ChessPieceSelectableState::onEnter(sf::Vector2i const &mousePos) noexcept
  {
    selectableTiles_ = gameRule_->getSelectableTiles();
    onMouseMoved(mousePos);

    // SPDLOG_INFO("Entered '{}'", typeid(*this).name());
  }

  void ChessPieceSelectableState::onExit() noexcept
  {
    choiceHighlighter_.getItem().setVisible(false);
    lastHoveredTile_ = {-1, -1};
  }

  void ChessPieceSelectableState::onMouseMoved(sf::Vector2i const &mousePos) noexcept
  {
    TileCoords tile = tileMap_->screenToTile(mousePos);
    if (tile == lastHoveredTile_)
    {
      return;
    }

    ItemPlacementMap::iterator found = selectableTiles_.find(tile);
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

  void ChessPieceSelectableState::onMousePressed(sf::Vector2i const &mousePos) noexcept
  {
    TileCoords targetedTile = tileMap_->screenToTile(mousePos);
    ItemPlacementMap::iterator found = selectableTiles_.find(targetedTile);
    if (found == selectableTiles_.end())
    {
      return;
    }

    std::shared_ptr<IBoardViewState>
        pieceSelectedState = std::make_shared<ChessPieceSelectedState>(
            gameBoard_, gameRule_, tileMap_, itemStore_, targetedTile);
    gameBoard_->pushState(pieceSelectedState, mousePos);
  }

  void ChessPieceSelectableState::onMouseReleased(sf::Vector2i const & /*mousePos*/) noexcept
  {
    ///< do nothing
  }

  void ChessPieceSelectableState::onServerMessage(ServerMessage const & /*msg*/) noexcept
  {
    ///< do nothing
  }
} // namespace bgg
