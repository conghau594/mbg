// SfPlayerSelectionScreen.h
#pragma once

#include <SFML/System/Time.hpp>

#include "SfBaseScreen.h"

struct ImFont;
namespace iab
{
  class SfPlayerSelectionScreen final : public SfBaseScreen
  {
    std::vector<std::string> playerTypeNames_;
    ImFont *smallFont_;
    int pressedButtonIndex_;
    int gameType_;

  public:
    SfPlayerSelectionScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<ScreenManager> screenMgr,
        std::vector<std::string> playerTypeNames,
        int gameType) noexcept;

  private:
    void update(sf::Time const &elapsed) noexcept override;

    void onWindowEventExceptClosed(std::optional<sf::Event> const &event) noexcept override;

    void doExit() noexcept override;
    void doEnter() noexcept override;

    void drawMenu() noexcept;
  };

} // namespace iab
