// GameAppFactory.h
#pragma once

namespace iab
{
  class GameApp;

  class GameAppFactory
  {
  public:
    virtual ~GameAppFactory() = default;
    virtual GameApp createGameApp() = 0;
  };
} // namespace iab
