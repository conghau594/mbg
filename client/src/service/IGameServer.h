// IGameServer.h
#pragma once

#include "ServerMessage.h"

namespace bgg
{
  class IGameServer
  {
  public:
    virtual ~IGameServer() = default;

    virtual void emit(ServerMessage const &msg) = 0;
  };
}
