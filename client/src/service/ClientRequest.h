// ClientRequest.h
#pragma once

#include <string>
#include <optional>
#include <map>

#include "model/Piece.h"
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

    Position fromSquare, toSquare;
    std::optional<std::string> promote; // e.g. promote to: "Queen", "check"
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