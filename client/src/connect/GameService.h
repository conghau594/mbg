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

    /**
     * \return gameId if succeeds
     *
     * \exception if failed to find opponent
     */
    virtual std::string findOpponent(int gameType, int playerType) = 0;
    virtual void resignGame(std::string const &gameId) = 0;
    virtual void commitMove(std::string const &gameId, Move const *const move) = 0;
  };
}
