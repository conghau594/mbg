// GameDisplay.h
#pragma once

#include "service/ClientRequest.h"

namespace std
{
  template <typename T>
  class shared_ptr;
}

namespace bgg
{
  class GameScreen;
  class GameDisplay
  {
  public:
    virtual ~GameDisplay() = default;
    virtual void run() = 0;
    virtual void send(ClientRequest const &request) = 0;

    virtual void pushScreen(std::shared_ptr<GameScreen> newScreen) = 0;
    virtual void popScreen() = 0;
    virtual void changeScreen(std::shared_ptr<GameScreen> newScreen) = 0;
  };

} // namespace bgg