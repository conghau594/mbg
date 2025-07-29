// ClientRequest.h
#pragma once

#include <string>
#include <optional>
#include <map>

#include "model/chess/ChessMove.h"
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

  class MoveRequest final
  {
  public:
    std::string userId;
    std::string gameId;

    ChessMove move;
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
      MoveRequest,
      ResignGameRequest>;

}