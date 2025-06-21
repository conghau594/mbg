// GameServiceSimulator.h
#pragma once

#include "GameService.h"

namespace boost
{
  namespace uuids
  {
    class random_generator;
  }
}

namespace iab
{
  class GameServiceSimulator : public GameService
  {
    boost::uuids::random_generator *uuidGenerator_;

  public:
    GameServiceSimulator();
    std::string findOpponent(int gameType, int playerType) override;
    void resignGame(std::string const &gameId) override;
    void commitMove(std::string const &gameId, Move const *const move) override;
  };

} // namespace iab
