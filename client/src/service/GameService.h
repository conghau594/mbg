// GameService.h
#pragma once

#include "ServerMessage.h"

namespace bgg
{
  class GameService
  {
  public:
    virtual ~GameService() = default;

    virtual void send(ServerMessage const &request) = 0;
  };
}
