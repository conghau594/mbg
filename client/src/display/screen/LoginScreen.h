// LoginScreen.h
#pragma once

#include <future>

#include "BaseScreen.h"
#include "MessageScreen.h"
#include "service/ServerMessage.h"

namespace bgg
{
  class LoginScreen final : public BaseScreen
  {
    std::shared_ptr<IDisplay> gameDisplay_;
    char usernameBuffer_[128];
    char passwordBuffer_[128];
    int pressedButtonIndex_;

  public:
    LoginScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<IDisplay> gameDisplay);
    ~LoginScreen();

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
