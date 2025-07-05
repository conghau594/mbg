// ClientEvent.h
#pragma once

#include <string>
#include "peeb/Event.hpp"

namespace bgg
{
  class LoginRequest final
  {
  public:
    std::string username;
    std::string password;
  };

  class FindGameRequest final
  {
  public:
    std::string userId;
    int gameType;
    int playerType;
  };

  class CancelMatchmakingRequest final
  {
  public:
    std::string userId;
  };

  class CommitMoveRequest final
  {
  public:
    std::string userId;
    std::string gameId;

    int pieceType;
    int side;

    struct
    {
      int x, y, z;
    } pos;
  };

  class ResignGameRequest final
  {
  public:
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