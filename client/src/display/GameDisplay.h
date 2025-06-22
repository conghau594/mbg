// GameDisplay.h
#pragma once

#include "ScreenManager.h"

namespace iab
{
  class GameDisplay : public ScreenManager
  {
  public:
    virtual ~GameDisplay() = default;
    virtual void run() = 0;
  };

} // namespace iab