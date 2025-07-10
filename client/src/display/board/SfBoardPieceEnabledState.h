// SfBoardPieceEnabledState.h
#pragma once

#include <memory>
#include <map>

#include "SfBoardState.h"
#include "SfItemStore.h"
#include "SfGameRuleAdapter.h"

namespace bgg
{
  class SfGameBoard;
  class SfTileMap;

  class SfBoardPieceEnabledState final : public SfBoardState
  {
    std::shared_ptr<SfGameBoard> gameBoard_;

    std::shared_ptr<SfGameRuleAdapter> gameRule_;
    std::shared_ptr<SfItemStore> itemStore_;
    std::shared_ptr<SfTileMap> tileMap_;

    SfItemPlacementMap selectableTiles_;
    SfItemStore::Entry choiceHighlighter_;
    TileCoords lastHoveredTile_;

  public:
    SfBoardPieceEnabledState(
        std::shared_ptr<SfGameBoard> gameBoard,
        std::shared_ptr<SfGameRuleAdapter> gameRule,
        std::shared_ptr<SfTileMap> tileMap,
        std::shared_ptr<SfItemStore> itemStore) noexcept;

    ~SfBoardPieceEnabledState() noexcept;

  private:
    void onEnter() noexcept override;
    void onExit() noexcept override;
    void onMouseMoved(sf::Vector2i const &mousePos) noexcept override;
    void onMousePressed(sf::Vector2i const &mousePos) noexcept override;
    void onMouseReleased(sf::Vector2i const &mousePos) noexcept override;
  };
}