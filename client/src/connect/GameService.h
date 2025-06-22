// GameService.h
#pragma once

#include <string>

namespace iab
{
  class Move;
  class GameService
  {
  public:
    virtual ~GameService() = default;

    virtual void connect(std::string const &userId) = 0;

    /**
     * \return gameId if succeeds
     *
     * \exception if failed to find opponent
     */
    virtual auto findOpponent(std::string const &userId, int gameType, int playerType) -> std::string = 0;
    virtual void cancelMatchmaking(std::string const &userId) = 0;
    virtual void resignGame(std::string const &userId, std::string const &gameId) = 0;
    virtual void commitMove(std::string const &userId, std::string const &gameId, Move const *const move) = 0;
  };
}
