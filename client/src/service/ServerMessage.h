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

  struct FindGameAcceptedNotification
  {
    ErrorCode errcode;
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

  struct GameStartedNotification
  {
    ErrorCode errcode;
  };

  struct GameUpdatedNotification
  {
    ErrorCode errcode;
  };

  struct GameFinishedNotification
  {
    ErrorCode errcode;
  };

  using ServerMessage = peeb::Event<
      LoginResponse,
      FindGameAcceptedNotification,
      FindGameResponse,
      CancelMatchmakingResponse,
      GameStartedNotification,
      CommitMoveResponse,
      ResignGameResponse,
      GameUpdatedNotification,
      GameFinishedNotification>;

} // namespace bgg
