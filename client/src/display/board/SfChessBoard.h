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
#include "SfTextureAtlas.h"

namespace sf
{
  class RenderWindow;
}
namespace bgg
{
  enum ZOrder
  {
    ZEROTH_LAYER, // Note that the 0th layer is reserved for tiles of the board
    FIRST_LAYER,
    SECOND_LAYER,
    THIRD_LAYER,
    FOURTH_LAYER,
    FIFTH_LAYER,
    SIXTH_LAYER,
    SEVENTH_LAYER,
    EIGHTH_LAYER,
    NINETH_LAYER,
    TENTH_LAYER,
  };

  class SfChessBoard final : public SfGameBoard
  {
    std::shared_ptr<sf::RenderWindow> window_;
    std::shared_ptr<SfBoardState> currentBoardState_;
    std::shared_ptr<SfBoardState> lastBoardState_;

    SfTextureAtlas atlas_;
    SfTileMap tileMap_;

    sf::Vector2i tileSizeOnScreen_; ///< changed when the window is resized

    sf::Vector2i boundaryTopLeft_; ///< fixed even the window is resized
    // sf::Vector2i boundaryBottomRight_; ///< fixed even the window is resized

    sf::Vector2i boardTopLeft_; ///< changed when the window is resized
    // sf::Vector2i boardBottomRight_; ///< changed when the window is resized

    std::map<size_t, SfBoardItem> boardItems_;
    std::atomic<std::size_t> nextBaseItemId_; ///< id generator for the boardItems_ (std::map )

    constexpr static int Z_ORDER_BIT_COUNT = 8;
    constexpr static int MAX_Z_ORDER = 1 << Z_ORDER_BIT_COUNT;

  public:
    SfChessBoard(std::shared_ptr<sf::RenderWindow> window,
                 sf::Vector2i boundaryTopLeft,
                 SfTextureAtlas atlas,
                 sf::Vector2i mapSizeInTiles,
                 std::vector<sf::Vector2i> const &tileLayout) noexcept;

  private:
    void onEvent(sf::Event const &event) noexcept override;
    void send(ClientEvent const &request) noexcept override;
    void draw(sf::RenderTarget &target, sf::RenderStates states) const noexcept override;
    void changeState(std::shared_ptr<SfBoardState> newState) noexcept override;

    auto addItem(SfBoardItem item, int zOrder, bool visible = true) noexcept
        -> std::pair<std::size_t, SfBoardItem *> override;
    auto addItem(
        sf::Vector2i const &tileCoords,
        sf::Vector2i const &textureCellIndex,
        int zOrder,
        std::string name = "",
        bool visible = true) noexcept -> std::pair<std::size_t, SfBoardItem *> override;
    auto removeItem(std::size_t itemId) noexcept -> bool override;
    auto putItemOnTile(SfBoardItem *item, sf::Vector2i const &tileCoords) noexcept
        -> bool override;

    auto generateNextId(int zOrder) noexcept -> size_t;

    auto screenToTile(sf::Vector2i const &screenCoords) noexcept -> sf::Vector2i;
    auto tileToScreenRect(sf::Vector2i const &tileCoords) noexcept -> sf::IntRect;
    void fitWindow() noexcept;
  };

} // namespace bgg
