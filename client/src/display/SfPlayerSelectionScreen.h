// SfPlayerSelectionScreen.h
#pragma once

#include "SfBaseScreen.h"

namespace iab
{
  class SfPlayerSelectionScreen final : public SfBaseScreen
  {
    sf::Time elapsedTime_;
    ImFont *smallFont_;
    int pressedButtonIndex_;
    int gameType_;

  public:
    SfPlayerSelectionScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameDisplay> displayContext,
        int gameType) noexcept;

    void onExit() noexcept override;
    void onEnter() noexcept override;

  private:
    void onTimeElapsed() noexcept override;
    void onEvent(std::optional<sf::Event> const &event) noexcept override;

    void drawMenu() noexcept;
    void update(sf::Time const &elapsedTime) noexcept;
  };

} // namespace iab
