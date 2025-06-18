// State.h
#pragma once

namespace base
{
  class State
  {
  public:
    virtual ~State() = default;
    virtual void onEnter() = 0;
    virtual void onExit() = 0;
  };
}