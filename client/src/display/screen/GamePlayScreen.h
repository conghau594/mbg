// GamePlayScreen.h
#pragma once

#include "BaseScreen.h"

namespace bgg
{
  class IDisplay;
  class IGameBoard;
  class GamePlayScreen final : public BaseScreen
  {
    std::shared_ptr<IDisplay> gameDisplay_;
    std::shared_ptr<IGameBoard> gameBoard_;
    int resignRegionHeight_;
    int pressedButtonIndex_;

  public:
    GamePlayScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<IDisplay> gameDisplay,
        std::shared_ptr<IGameBoard> gameBoard,
        int resignRegionHeight) noexcept;

  private:
    void update(sf::Time const &elapsed) noexcept override;

    void onWindowEventExceptClosed(
        std::optional<sf::Event> const &event) noexcept override;

    void onGameFinishedNotification(
        GameFinishedNotification const &notif) noexcept;

    void doEnter() noexcept override;
    void doExit() noexcept override;

    void layOutScreen() noexcept;
  };
}