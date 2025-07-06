// SfBaseScreen.h
#pragma once

#include <memory>
#include <SFML/System/Clock.hpp>

#include "GameScreen.h"
#include "peeb/Handler.hpp"
#include "service/ServerMessage.h"

namespace std
{
  template <typename T>
  class optional;
}

namespace sf
{
  class RenderWindow;
  class Clock;
  class Event;
  class Time;
}

namespace bgg
{
  /////////////////////////////////////////////////////////////////////////////
  class GameDisplay;
  class SfBaseScreen
      : public std::enable_shared_from_this<SfBaseScreen>,
        public GameScreen
  {
    std::shared_ptr<sf::RenderWindow> window_;
    std::shared_ptr<GameScreen> lastSubscreen_;
    std::shared_ptr<GameScreen> currentSubscreen_;

    peeb::Handler<ServerMessage> serverMessageHandler_;
    sf::Clock clock_;
    bool isActive_;

  protected:
    SfBaseScreen(std::shared_ptr<sf::RenderWindow> window) noexcept;

    virtual void update(sf::Time const &elapsed) = 0;

    virtual void onWindowEventExceptClosed(std::optional<sf::Event> const &event) = 0;
    virtual void onWindowClosed();

    virtual void doEnter() = 0;
    virtual void doExit() = 0;

    [[nodiscard]] auto getWindow() const noexcept
        -> std::shared_ptr<sf::RenderWindow> const & { return window_; }

    [[nodiscard]] auto getServerMsgHandler() noexcept
        -> peeb::Handler<ServerMessage> & { return serverMessageHandler_; }

    static void askExitConfirmation(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<SfBaseScreen> parentScreen,
        std::string const &msg) noexcept;

    void changeSubscreen(std::shared_ptr<GameScreen> newSubscreen) noexcept override final;

  private:
    void update() override final;
    void onEnter() override final;
    void onExit() override final;
    void handleServerMessages(std::list<ServerMessage> &messages) noexcept override final;

    [[nodiscard]] auto isActive() const noexcept
        -> bool override final { return isActive_; }
    void deactivate() noexcept override final { isActive_ = false; }
  };
}