// GameScreen.h
#pragma once

#include <list>
#include <memory>
#include "service/ServerMessage.h"
namespace bgg
{
  class GameScreen
  {
  public:
    virtual ~GameScreen() = default;
    virtual void update() = 0;
    virtual void deactivate() = 0;
    [[nodiscard]] virtual auto isActive() const -> bool = 0;
    virtual void onExit() = 0;
    virtual void onEnter() = 0;
    virtual void handleServerMessages(std::list<ServerMessage> &messages) = 0;
    virtual void changeSubscreen(std::shared_ptr<GameScreen> newSubscreen) = 0;
  };
}