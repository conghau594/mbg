// GameScreen.h
#pragma once

namespace iac
{
  class GameScreen
  {
  public:
    virtual ~GameScreen() = default;
    virtual void update() = 0;
    virtual void onExit() = 0;
    virtual void onEnter() = 0;
  };
}