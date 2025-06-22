// GameServiceSimulator.h
#pragma once

#include <memory>

#include "GameService.h"
#include "base/ThreadPool.h"

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
    util::ThreadPool threadPool_;
    std::shared_ptr<boost::uuids::random_generator> uuidGenerator_;
    bool isConnected_;

  public:
    GameServiceSimulator() noexcept;

    void connect(std::string const &userId, Callback const &callback) override;

    void disconnect() noexcept override;

    auto isConnected() const noexcept -> bool override;

    [[nodiscard]] auto findOpponent(
        std::string const &userId,
        int gameType,
        int playerType,
        Callback const &callback) noexcept -> std::string override;

    void cancelMatchmaking(
        std::string const &userId, Callback const &callback) noexcept override;

    void resignGame(
        std::string const &userId,
        std::string const &gameId,
        Callback const &callback) noexcept override;

    void commitMove(
        std::string const &userId,
        std::string const &gameId,
        Move const *const move,
        Callback const &callback) noexcept override;

  private:
    void simulateNetworkLatencyAndFailure(
        int failurePercent = 0, int minDelay = 100, int maxDelay = 2000);
  };

} // namespace iab
