// ServerMessage.h
#pragma once

#include <string>
#include <optional>
#include <map>

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
  class FindGameAcceptedNotification final
  {
  public:
    ErrorCode errcode;
    std::string userId;
    int gameType;
    int playerType;
  };

  struct Position
  {
    int x, y, z;
  };

  using ItemIndex = int;

  class FindGameResponse final
  {
  public:
    ErrorCode errcode;
    std::string userId;
    std::string gameId;

    // TODO: consider how to use these variables
    int gameType;
    int playerType;

    std::map<ItemIndex, Position> initialBoard;
    int yourSide;
    int yourTurn;
    int currentTurn;
  };

  class CancelMatchmakingResponse final
  {
  public:
    ErrorCode errcode;
  };

  class GameUpdatedNotification final
  {
  public:
    std::string userId;
    std::string gameId;
    std::map<ItemIndex, Position> currentBoard;
    int yourTurn;
    int currentTurn;
  };

  class GameFinishedNotification final
  {
  public:
    std::string userId;
    std::string gameId;
    std::map<ItemIndex, Position> currentBoard;
    int yourRank;
    int playerCount;
  };

  class CommitMoveResponse final
  {
  public:
    ErrorCode errcode;
    std::optional<GameUpdatedNotification> currentGameState;
  };

  class ResignGameResponse final
  {
  public:
    ErrorCode errcode;
    std::string userId;
    std::string gameId;
  };

  using ServerMessage = peeb::Event<
      LoginResponse,
      FindGameAcceptedNotification,
      FindGameResponse,
      CancelMatchmakingResponse,
      CommitMoveResponse,
      ResignGameResponse,
      GameUpdatedNotification,
      GameFinishedNotification>;

} // namespace bgg
