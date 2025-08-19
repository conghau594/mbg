// IGameServer.h
#pragma once

#include "ClientRequest.h"
#include "IGameServer.h"

namespace bgg
{
  class IGameService
  {
  public:
    virtual ~IGameService() = default;

    virtual void handleRequest(ResignGameRequest const &request) = 0;
    virtual void handleRequest(MoveRequest const &request) = 0;
  };
}
