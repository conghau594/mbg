// IScreen.h
#pragma once

#include <list>
#include <memory>
#include "service/ServerMessage.h"
namespace bgg
{
  class IScreen
  {
  public:
    virtual ~IScreen() = default;

    virtual void update() = 0;
    virtual void onExit() = 0;
    virtual void onEnter() = 0;
    virtual void handleServerMessages(std::list<ServerMessage> &messages) = 0;
  };
}