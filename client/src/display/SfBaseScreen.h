// SfBaseScreen.h
#pragma once

#include <memory>
#include <list>

#include "GameScreen.h"
#include "ScreenManager.h"

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

namespace iab
{
  class GameService;
  class SfBaseScreen
      : public std::enable_shared_from_this<SfBaseScreen>,
        public GameScreen,
        public ScreenManager
  {
    std::list<std::shared_ptr<GameScreen>> screenStack_;
    std::shared_ptr<GameScreen> currentScreen_;
    std::shared_ptr<sf::RenderWindow> window_;
    std::shared_ptr<GameService> gameService_;
    std::shared_ptr<sf::Clock> clock_;
    std::shared_ptr<ScreenManager> parentScreenMgr_;
    bool isActive_;

  public:
    SfBaseScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameService> gameService,
        std::shared_ptr<ScreenManager> screenMgr) noexcept;

    void update() override final;
    void onEnter() override final;
    void onExit() override final;

    [[nodiscard]] auto isActive() const noexcept
        -> bool override final { return isActive_; }
    void deactivate() noexcept override final { isActive_ = false; }

    void pushScreen(std::shared_ptr<GameScreen> newScreen) noexcept override;
    void popScreen() noexcept override;
    void changeScreen(std::shared_ptr<GameScreen> newScreen) noexcept override;

  protected:
    virtual void update(sf::Time const &elapsed) = 0;

    virtual void onWindowEventExceptClosed(std::optional<sf::Event> const &event) = 0;
    virtual void onWindowClosed();

    virtual void doEnter() = 0;
    virtual void doExit() = 0;

    [[nodiscard]] auto window() const noexcept
        -> std::shared_ptr<sf::RenderWindow> const & { return window_; }
    [[nodiscard]] auto gameService() const noexcept
        -> std::shared_ptr<GameService> const & { return gameService_; }
    [[nodiscard]] auto parentScreenManager() const noexcept
        -> std::shared_ptr<ScreenManager> const & { return parentScreenMgr_; }
    [[nodiscard]] auto screenStack() const noexcept
        -> std::list<std::shared_ptr<GameScreen>> const & { return screenStack_; }
    [[nodiscard]] auto currentScreen() const noexcept
        -> std::shared_ptr<GameScreen> const & { return currentScreen_; }

    static void askExitConfirmation(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameService> gameService,
        std::shared_ptr<ScreenManager> screenMgr,
        std::string const &msg) noexcept;
  };
}