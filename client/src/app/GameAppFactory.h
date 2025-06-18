// GameAppFactory.h
#pragma once

namespace iac
{
  class GameApp;

  class GameAppFactory
  {
  public:
    virtual ~GameAppFactory() = default;
    virtual GameApp createGameApp() = 0;
  };
} // namespace iac
