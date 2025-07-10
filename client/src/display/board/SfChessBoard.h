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
    std::list<std::shared_ptr<SfBoardState>> stateStack_;
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
    void commitAction(BoardAction const &action) noexcept override;
    void handleServerMessage(ServerMessage const& msg) noexcept override;

    void draw(
        sf::RenderTarget &target,
        sf::RenderStates states) const noexcept override;
    void changeState(std::shared_ptr<SfBoardState> newState,
                     sf::Vector2i const &mousePos) noexcept override;

    void pushState(std::shared_ptr<SfBoardState> newState,
                   sf::Vector2i const &mousePos) noexcept override;
    void popState(sf::Vector2i const &mousePos) noexcept override;
    void clearStates() noexcept override;

    void fitRectangle(sf::IntRect const &boardRect) noexcept;

    //
    void onGameUpdatedNotification(GameUpdatedNotification const &notif) noexcept;
    void onGameFinishedNotification(GameFinishedNotification const &notif) noexcept;
    void onCommitMoveResponse(CommitMoveResponse const &response) noexcept;
  };

} // namespace bgg
