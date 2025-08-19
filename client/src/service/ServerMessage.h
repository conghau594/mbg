// ServerMessage.h
#pragma once

#include <string>
#include <optional>
#include <map>

#include "peeb/Event.hpp"
#include "base/ErrorCode.h"
#include "model/chess/ChessMove.h"

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

    std::list<std::tuple<EntityType, Side, Position>> initialPlacements;
    Side yourSide;
    Side currentTurn;
  };

  class CancelMatchmakingResponse final
  {
  public:
    ErrorCode errcode;
  };

  class GameUpdatedNotification final
  {
  public:
    ChessMove::Detail opponentMoveDetail;
    Side yourSide;
    Side currentTurn;
  };

  class GameFinishedNotification final
  {
  public:
    std::string result; // "Win", "Lose", "Draw", "Error"

    // int yourRank;
    // int playerCount;
  };

  class MoveResponse final
  {
  public:
    ErrorCode errcode;
    // std::optional<GameUpdatedNotification> currentGameState;
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
      MoveResponse,
      ResignGameResponse,
      GameUpdatedNotification,
      GameFinishedNotification>;

} // namespace bgg
