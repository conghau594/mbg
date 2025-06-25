// SfBaseScreen.h
#pragma once

#include <memory>
#include <mutex>

#include "GameScreen.h"

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
  class GameDisplay;
  class SfBaseScreen
      : public std::enable_shared_from_this<SfBaseScreen>,
        public GameScreen
  {
    std::shared_ptr<sf::RenderWindow> window_;
    std::shared_ptr<GameDisplay> gameDisplay_;
    std::shared_ptr<GameService> gameService_;

    std::shared_ptr<GameScreen> lastSubscreen_;
    std::shared_ptr<GameScreen> currentSubscreen_;
    std::mutex subscreenMutex_;
    std::shared_ptr<sf::Clock> clock_;
    bool isActive_;

  public:
    SfBaseScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameService> gameService,
        std::shared_ptr<GameDisplay> gameDisplay) noexcept;

    void update() override final;
    void onEnter() override final;
    void onExit() override final;

    [[nodiscard]] auto isActive() const noexcept
        -> bool override final { return isActive_; }
    void deactivate() noexcept override final { isActive_ = false; }

    void changeSubscreen(std::shared_ptr<GameScreen> newSubscreen) noexcept override;

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
    [[nodiscard]] auto gameDisplay() const noexcept
        -> std::shared_ptr<GameDisplay> const & { return gameDisplay_; }

    static void askExitConfirmation(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameService> gameService,
        std::shared_ptr<GameDisplay> gameDisplay,
        std::shared_ptr<SfBaseScreen> parentScreen,
        std::string const &msg) noexcept;
  };
}