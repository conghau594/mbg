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
#include "ZOrder.h"

#include "ChessTextureCell.h"

namespace sf
{
  class RenderWindow;
}
namespace bgg
{
  class SfChessBoard final : public SfGameBoard
  {
    std::shared_ptr<SfBoardState> currentBoardState_;
    std::shared_ptr<SfBoardState> lastBoardState_;

    std::shared_ptr<const SfTextureAtlas> const itemTextureAtlas_;
    SfTileMap tileMap_;

    sf::Vector2i paddingTopLeft_;     ///< fixed even the window is resized
    sf::Vector2i paddingBottomRight_; ///< fixed even the window is resized

    // sf::Vector2i boardTopLeft_; ///< changed when the window is resized
    // // sf::Vector2i boardBottomRight_; ///< changed when the window is resized

    std::map<size_t, SfBoardItem> boardItems_;
    std::atomic<std::size_t> nextBaseItemId_; ///< id generator for the boardItems_ (std::map )

    constexpr static int Z_ORDER_BIT_COUNT = 8;
    constexpr static int MAX_Z_ORDER = 1 << Z_ORDER_BIT_COUNT;

  public:
    SfChessBoard(sf::Vector2i const &currentWndSize,
                 sf::Vector2i paddingTopLeft,
                 sf::Vector2i paddingBottomRight,
                 std::shared_ptr<const SfTextureAtlas> itemTextureAtlas,
                 SfTileMap tileMap) noexcept;

  private:
    void onEvent(sf::Event const &event) noexcept override;
    void send(ClientEvent const &request) noexcept override;
    void draw(sf::RenderTarget &target, sf::RenderStates states) const noexcept override;
    void changeState(std::shared_ptr<SfBoardState> newState) noexcept override;

    auto addItem(SfBoardItem item, int zOrder, bool visible = true) noexcept
        -> std::pair<const std::size_t, SfBoardItem> * override;
    auto addItem(
        sf::Vector2i const &tileCoords,
        int textureCellIndex,
        int zOrder,
        std::string name = "",
        bool visible = true) noexcept
        -> std::pair<const std::size_t, SfBoardItem> * override;
    auto removeItem(std::size_t itemId) noexcept -> bool override;
    auto putItemOnTile(SfBoardItem *item, sf::Vector2i const &tileCoords) noexcept
        -> bool override;

    auto generateNextId(int zOrder) noexcept -> size_t;

    void fitWindow(sf::Vector2i const &wndSize) noexcept;
  };

} // namespace bgg
