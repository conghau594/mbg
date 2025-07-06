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
#include "SfItemStore.h"
#include "ZOrder.h"

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

    SfItemStore itemStore_;
    SfTileMap tileMap_;

    sf::Vector2i paddingTopLeft_;     ///< fixed even the window is resized
    sf::Vector2i paddingBottomRight_; ///< fixed even the window is resized

  public:
    SfChessBoard(sf::Vector2i const &currentWndSize,
                 sf::Vector2i paddingTopLeft,
                 sf::Vector2i paddingBottomRight,
                 SfItemStore itemStore_,
                 SfTileMap tileMap) noexcept;

  private:
    void onEvent(sf::Event const &event) noexcept override;
    void send(ClientEvent const &request) noexcept override;
    void draw(sf::RenderTarget &target, sf::RenderStates states) const noexcept override;
    void changeState(std::shared_ptr<SfBoardState> newState) noexcept override;

    void fitWindow(sf::Vector2i const &wndSize) noexcept;
  };

} // namespace bgg
