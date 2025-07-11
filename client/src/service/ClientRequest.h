// ClientRequest.h
#pragma once

#include <string>
#include <map>

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

    struct
    {
      int x, y;
    } fromPosition, toPosition;

    // TODO: should refactor this
    std::map<std::string, std::string> specialMove; // e.g. promotion: "Queen"
  };

  class ResignGameRequest final
  {
  public:
    std::string userId;
    std::string gameId;
  };

  using ClientRequest = peeb::Event<
      LoginRequest,
      FindGameRequest,
      CancelMatchmakingRequest,
      CommitMoveRequest,
      ResignGameRequest>;

}