// SfLoginScreen.h
#pragma once

#include <future>

#include "SfBaseScreen.h"
#include "SfMessageScreen.h"
#include "service/Response.h"

namespace iab
{
  class SfConnectionWaitingScreen : public SfMessageScreen
  {
    std::future<Response> const &futureLoginResponse_;
    std::shared_ptr<GameScreen> parentScreen_;

  public:
    SfConnectionWaitingScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameScreen> parentScreen,
        std::future<Response> const &futureLoginResponse) noexcept;

  private:
    void update(sf::Time const &elapsed) noexcept override;
  };

  class SfLoginScreen final : public SfBaseScreen
  {
    std::shared_ptr<GameDisplay> gameDisplay_;
    std::future<Response> futureLoginResponse_;
    char usernameBuffer_[128];
    char passwordBuffer_[128];
    int pressedButtonIndex_;

  public:
    SfLoginScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameDisplay> gameDisplay) noexcept;

  private:
    void update(sf::Time const &elapsedTime) noexcept override;

    void onWindowEventExceptClosed(std::optional<sf::Event> const &event) noexcept override;

    void doExit() noexcept override;
    void doEnter() noexcept override;

    void layOutScreen() noexcept;
    void sendLoginRequest() noexcept;
  };
} // namespace iab
