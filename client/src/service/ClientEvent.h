// ClientEvent.h
#pragma once

#include <string>
#include "peeb/Event.hpp"

namespace bgg
{
  struct LoginRequest
  {
    std::string username;
    std::string password;
  };

  struct FindGameRequest
  {
    std::string userId;
    int gameType;
    int playerType;
  };

  struct CancelMatchmakingRequest
  {
    std::string userId;
  };

  struct CommitMoveRequest
  {
    std::string userId;
    std::string gameId;

    int pieceType;
    int side;

    struct
    {
      int x, y, z;
    } pos;
  };

  struct ResignGameRequest
  {
    std::string userId;
    std::string gameId;
  };

  using ClientEvent = peeb::Event<
      LoginRequest,
      FindGameRequest,
      CancelMatchmakingRequest,
      CommitMoveRequest,
      ResignGameRequest>;

}