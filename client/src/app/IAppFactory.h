// IAppFactory.h
#pragma once

namespace bgg
{
  class GameApp;

  class IAppFactory
  {
  public:
    virtual ~IAppFactory() = default;
    [[nodiscard]] virtual auto createGameApp() -> GameApp = 0;
  };
} // namespace bgg
