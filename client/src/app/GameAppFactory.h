// GameAppFactory.h
#pragma once

namespace bgg
{
  class GameApp;

  class GameAppFactory
  {
  public:
    virtual ~GameAppFactory() = default;
    virtual auto createGameApp() -> GameApp = 0;
  };
} // namespace bgg
