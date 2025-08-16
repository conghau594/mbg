// GameSelectionScreen.h
#pragma once

#include <vector>
#include <string>

#include "BaseScreen.h"
#include "GameFindingScreen.h"

namespace bgg
{
  class SideSelectionScreen final : public BaseScreen
  {
    std::shared_ptr<IDisplay> gameDisplay_;
    std::vector<std::string> sideNames_;
    int pressedButtonIndex_;
    int gameType_;
    int playerType_;
    int side_;

  public:
    SideSelectionScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<IDisplay> gameDisplay,
        std::vector<std::string> sideNames,
        int gameType,
        int playerType) noexcept;

  private:
    void update(sf::Time const &elapsedTime) noexcept override;

    void onWindowEventExceptClosed(
        std::optional<sf::Event> const &event) noexcept override;

    void doExit() noexcept override;
    void doEnter() noexcept override;

    void sendFindGameRequest() noexcept;
    void layOutScreen() noexcept;
    void onFindGameAcceptedNotification(
        FindGameAcceptedNotification const &response) noexcept;
  };

} // namespace bgg
