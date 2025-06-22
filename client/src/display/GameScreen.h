// GameScreen.h
#pragma once

namespace iab
{
  class GameScreen
  {
  public:
    virtual ~GameScreen() = default;
    virtual void update() = 0;
    virtual void deactivate() = 0;
    virtual auto isActive() const -> bool = 0;
    virtual void onExit() = 0;
    virtual void onEnter() = 0;
    virtual void changeSubscreen(std::shared_ptr<GameScreen> newSubscreen) = 0;
  };
}