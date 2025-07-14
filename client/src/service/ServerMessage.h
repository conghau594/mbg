// ServerMessage.h
#pragma once

#include <string>
#include <optional>
#include <map>

#include "peeb/Event.hpp"
#include "base/ErrorCode.h"
#include "model/Piece.h"

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

  class FindGameResponse final
  {
  public:
    ErrorCode errcode;
    int gameType;

    std::map<Position, Piece> initialBoard;
    std::string yourSide;
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
    // enemy move
    Position fromPosition, toPosition;
    std::optional<std::string> promote; // e.g. promote to: "Queen"
    int yourTurn;
    int currentTurn;
  };

  class GameFinishedNotification final
  {
  public:
    std::map<Position, Piece> currentBoard;
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
