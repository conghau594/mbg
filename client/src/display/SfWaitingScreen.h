// SfWaitingScreen.h
#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <string>

#include "SfBaseScreen.h"

namespace iab
{
  class SfBlockingScreen;

  class SfWaitingScreen final : public SfBaseScreen
  {
    std::shared_ptr<SfBlockingScreen> blockingScreen_;

  public:
    SfWaitingScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<ScreenManager> screenMgr,
        std::string const &message,
        std::vector<std::string> buttonLabels,
        std::vector<std::function<void()>> buttonCallbacks) noexcept;

  private:
    void update(sf::Time const &elapsed) noexcept override;

    void onWindowEventExceptClosed(std::optional<sf::Event> const &event) noexcept override;

    void doExit() noexcept override;
    void doEnter() noexcept override;
  };
}