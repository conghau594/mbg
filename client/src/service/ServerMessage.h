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
  };

  struct Position
  {
    int x, y;
  };

  using ItemIndex = int;

  class FindGameResponse final
  {
  public:
    ErrorCode errcode;
    int gameType;

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
    std::map<ItemIndex, Position> currentBoard;
    int yourTurn;
    int currentTurn;
  };

  class GameFinishedNotification final
  {
  public:
    std::map<ItemIndex, Position> currentBoard;
    int yourRank;
    int playerCount;
  };

  class CommitMoveResponse final
  {
  public:
    ErrorCode errcode;

    std::map<std::string, std::string> specialMove; // e.g. promotion: "Queen"
    struct
    {
      int x, y;
    } fromPosition, toPosition;

    std::optional<GameUpdatedNotification> currentGameState;
  };

  class ResignGameResponse final
  {
  public:
    ErrorCode errcode;
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
