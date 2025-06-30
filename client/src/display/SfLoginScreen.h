// SfLoginScreen.h
#pragma once

#include <future>

#include "SfBaseScreen.h"
#include "SfMessageScreen.h"
#include "service/ServerMessage.h"

namespace bgg
{
  class SfLoginScreen final : public SfBaseScreen
  {
    std::shared_ptr<GameDisplay> gameDisplay_;
    char usernameBuffer_[128];
    char passwordBuffer_[128];
    int pressedButtonIndex_;

  public:
    SfLoginScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameDisplay> gameDisplay);
    ~SfLoginScreen();

  private:
    void update(sf::Time const &elapsedTime) noexcept override;

    void onWindowEventExceptClosed(std::optional<sf::Event> const &event) noexcept override;

    void doExit() noexcept override;
    void doEnter() noexcept override;

    void layOutScreen() noexcept;

    void sendLoginRequest() noexcept;
    void onLoginResponse(LoginResponse const &response) noexcept;
  };
} // namespace bgg
