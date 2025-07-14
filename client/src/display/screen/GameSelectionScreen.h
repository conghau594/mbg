// GameSelectionScreen.h
#pragma once

#include <vector>
#include <string>

#include "BaseScreen.h"

namespace bgg
{
  class GameSelectionScreen final : public BaseScreen
  {
    std::shared_ptr<IDisplay> gameDisplay_;
    std::vector<std::string> gameNames_;
    int pressedButtonIndex_;

  public:
    GameSelectionScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<IDisplay> gameDisplay,
        std::vector<std::string> gameNames) noexcept;

  private:
    void update(sf::Time const &elapsedTime) noexcept override;

    void onWindowEventExceptClosed(std::optional<sf::Event> const &event) noexcept override;

    void doExit() noexcept override;
    void doEnter() noexcept override;

    void layOutScreen() noexcept;
  };

} // namespace bgg
