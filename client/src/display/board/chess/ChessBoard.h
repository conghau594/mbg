// ChessBoard.h
#pragma once

#include <memory>
#include <map>
#include <atomic>
#include <functional>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include "IChessBoard.h"
#include "display/board/TileMap.h"
#include "display/board/ItemStore.h"
#include "display/board/IBoardState.h"
#include "display/board/ZOrder.h"

#include "service/ClientRequest.h"
#include "IChessRuleAdapter.h"

namespace sf
{
  class RenderWindow;
}
namespace bgg
{
  class ChessBoard final
      : public IChessBoard,
        public std::enable_shared_from_this<IChessBoard>
  {
    std::list<std::shared_ptr<IBoardState>> stateStack_;
    std::shared_ptr<IBoardState> lastBoardState_;

    std::shared_ptr<IChessRuleAdapter> gameRule_;
    std::shared_ptr<TileMap> tileMap_;
    std::shared_ptr<ItemStore> itemStore_;

    ItemStore::Entry lastMoveHighlighters_[2];

    std::function<void(ClientRequest const &)> requestSender_;

  public:
    ChessBoard(
        sf::IntRect const &boardRect,
        std::shared_ptr<IChessRuleAdapter> gameRule,
        std::shared_ptr<TileMap> tileMap,
        std::shared_ptr<ItemStore> itemStore,
        std::function<void(ClientRequest const &)> requestSender) noexcept;

  private:
    void onWindowEvent(sf::Event const &event) noexcept override;
    void sendMoveRequest(MoveRequest const &move) noexcept override;
    void handleServerMessage(ServerMessage const &msg) noexcept override;

    void draw(
        sf::RenderTarget &target,
        sf::RenderStates states) const noexcept override;

    void changeState(
        std::shared_ptr<IBoardState> newState,
        sf::Vector2i const &mousePos = FARTHEST_POSITION) noexcept override;
    void pushState(
        std::shared_ptr<IBoardState> newState,
        sf::Vector2i const &mousePos = FARTHEST_POSITION) noexcept override;
    void popState(
        sf::Vector2i const &mousePos = FARTHEST_POSITION) noexcept override;
    void clearStates() noexcept override;

    void fitRectangle(sf::IntRect const &boardRect) noexcept;
    void onGameFinishedNotification(
        GameFinishedNotification const &notif) noexcept;

    // void onGameUpdatedNotification(GameUpdatedNotification const &notif) noexcept;
    // void onMoveResponse(MoveResponse const &response) noexcept;
  };

} // namespace bgg
