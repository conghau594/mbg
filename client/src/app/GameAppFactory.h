// GameAppFactory.h
#pragma once

namespace iab
{
  class GameApp;

  class GameAppFactory
  {
  public:
    virtual ~GameAppFactory() = default;
    virtual auto createGameApp() -> GameApp = 0;
  };
} // namespace iab
