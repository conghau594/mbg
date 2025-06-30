// ServerMessage.h
#pragma once

#include <string>

#include "peeb/Event.hpp"
#include "base/ErrorCode.h"

namespace bgg
{

  struct LoginResponse
  {
    ErrorCode errcode;
    std::string userId;
  };

  struct FindGameResponse
  {
    ErrorCode errcode;
    std::string gameId;
    int side;
  };

  struct CancelMatchmakingResponse
  {
    ErrorCode errcode;
  };

  struct CommitMoveResponse
  {
    ErrorCode errcode;
  };

  struct ResignGameResponse
  {
    ErrorCode errcode;
  };

  struct GameStartedEvent
  {
    ErrorCode errcode;
  };

  struct GameUpdatedEvent
  {
    ErrorCode errcode;
  };

  using ServerMessage = peeb::Event<
      LoginResponse,
      FindGameResponse,
      CancelMatchmakingResponse,
      CommitMoveResponse,
      ResignGameResponse,
      GameStartedEvent,
      GameUpdatedEvent>;

} // namespace bgg
