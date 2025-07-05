// ServerMessage.h
#pragma once

#include <string>

#include "peeb/Event.hpp"
#include "base/ErrorCode.h"

namespace bgg
{

  class LoginResponse final
  {
  public:
    ErrorCode errcode;
    std::string userId;
  };

  class FindGameResponse final
  {
  public:
    ErrorCode errcode;
    std::string gameId;
    int side;
  };

  class FindGameAcceptedNotification final
  {
  public:
    ErrorCode errcode;
  };

  class CancelMatchmakingResponse final
  {
  public:
    ErrorCode errcode;
  };

  class CommitMoveResponse final
  {
  public:
    ErrorCode errcode;
  };

  class ResignGameResponse final
  {
  public:
    ErrorCode errcode;
  };

  class GameStartedNotification final
  {
  public:
    ErrorCode errcode;
  };

  class GameUpdatedNotification final
  {
  public:
    ErrorCode errcode;
  };

  class GameFinishedNotification final
  {
  public:
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
