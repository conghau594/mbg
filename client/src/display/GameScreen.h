// GameScreen.h
#pragma once

namespace iab
{
  class GameScreen
  {
  public:
    virtual ~GameScreen() = default;
    virtual void update() = 0;
    virtual void exit() = 0;
    virtual auto shouldExit() -> bool = 0;
    virtual void onExit() = 0;
    virtual void onEnter() = 0;
  };
}