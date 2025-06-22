// GameServiceSimulator.h
#pragma once

#include <memory>

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
    std::shared_ptr<boost::uuids::random_generator> uuidGenerator_;

  public:
    GameServiceSimulator();
    void connect(std::string const &userId) override;
    [[nodiscard]] auto findOpponent(std::string const &userId, int gameType, int playerType) -> std::string override;
    void cancelMatchmaking(std::string const &userId) override;
    void resignGame(std::string const &userId, std::string const &gameId) override;
    void commitMove(std::string const &userId, std::string const &gameId, Move const *const move) override;

  private:
    static void simulateNetworkLatencyAndFailure(
        int failurePercent = 0, int minDelay = 100, int maxDelay = 2000) noexcept;
  };

} // namespace iab
