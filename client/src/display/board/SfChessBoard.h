// SfChessBoard.h
#pragma once

#include <memory>
#include <map>
#include <atomic>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include "SfGameBoard.h"
#include "SfTileMap.h"
#include "SfBoardState.h"
#include "SfBoardItem.h"

namespace bgg
{
  class SfChessBoard final : public SfGameBoard
  {
    std::shared_ptr<SfBoardState> currentBoardState_;
    std::shared_ptr<SfBoardState> lastBoardState_;
    std::map<size_t, SfBoardItem> boardItems_;

    sf::Texture texture_;
    sf::Vector2i textureCellSize_;
    SfTileMap tileMap_;
    std::atomic<std::size_t> nextBaseId_;

    constexpr static int Z_ORDER_BIT_COUNT = 8;
    constexpr static int MAX_Z_ORDER = 1 << Z_ORDER_BIT_COUNT;

  public:
    enum ZOrder
    {
      FIRST_LAYER, // Note that the tiles are at the 0th layer
      SECOND_LAYER,
      THIRD_LAYER,
      FOURTH_LAYER,
      FIFTH_LAYER,
    };
    SfChessBoard(SfTileMap tileMap) noexcept;

  private:
    void onEvent(sf::Event const &event) noexcept override;
    void send(ClientEvent const &request) noexcept override;
    void draw(sf::RenderTarget &target, sf::RenderStates states) const noexcept override;
    void changeState(std::shared_ptr<SfBoardState> newState) noexcept override;

    auto addItem(SfBoardItem item, int zOrder, bool visible = true) noexcept
        -> std::pair<std::size_t, SfBoardItem *> override;
    auto addItem(
        sf::Vector2i tileCoords, int textureCellIndex, int zOrder, bool visible = true) noexcept
        -> std::pair<std::size_t, SfBoardItem *> override;
    auto removeItem(std::size_t itemId) noexcept -> bool override;
    auto putItemOnTile(SfBoardItem *item, sf::Vector2i tileCoords) noexcept
        -> bool override;

    auto generateNextId(int zOrder) noexcept -> size_t;
  };

} // namespace bgg
