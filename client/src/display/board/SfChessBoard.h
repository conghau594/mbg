// SfChessBoard.h
#pragma once

#include <memory>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include "SfGameBoard.h"
#include "SfTileMap.h"
#include "SfBoardState.h"

namespace bgg
{
  class SfChessBoard final : public SfGameBoard
  {
    std::shared_ptr<SfBoardState> currentBoardState_;
    std::shared_ptr<SfBoardState> lastBoardState_;
    SfTileMap tileMap_;

  public:
    SfChessBoard(SfTileMap tileMap) noexcept;

  private:
    void onEvent(sf::Event const &event) noexcept override;

    void send(ClientEvent const &request) noexcept override;

    void draw(sf::RenderTarget &target, sf::RenderStates states) const noexcept override;

    void changeState(std::shared_ptr<SfBoardState> newState) noexcept override;

    auto addItem(
        sf::Vector2i tileCoords, size_t zOrder, int textureCellIndex, bool visible = true) noexcept
        -> SfItem * override;

    auto removeItem(SfItem *item) noexcept -> bool  override;
    auto putItemOnTile(SfItem *item, sf::Vector2i tileCoords) noexcept -> bool override;
  };

} // namespace bgg
