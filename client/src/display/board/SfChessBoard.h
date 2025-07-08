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
  class SfGameRuleAdapter;
  class SfChessBoard final : public SfGameBoard
  {
    std::shared_ptr<SfBoardState> currentBoardState_;
    std::shared_ptr<SfBoardState> lastBoardState_;

    std::shared_ptr<SfGameRuleAdapter> gameRule_;
    std::shared_ptr<SfTileMap> tileMap_;
    std::shared_ptr<SfItemStore> itemStore_;

  public:
    SfChessBoard(
        sf::IntRect const &boardRect,
        std::shared_ptr<SfGameRuleAdapter> gameRule,
        std::shared_ptr<SfTileMap> tileMap,
        std::shared_ptr<SfItemStore> itemStore) noexcept;

  private:
    void onEvent(sf::Event const &event) noexcept override;
    void send(ClientEvent const &request) noexcept override;
    void draw(
        sf::RenderTarget &target,
        sf::RenderStates states) const noexcept override;
    void changeState(
        std::shared_ptr<SfBoardState> newState) noexcept override;

    void fitRectangle(sf::IntRect const &boardRect) noexcept;
  };

} // namespace bgg
