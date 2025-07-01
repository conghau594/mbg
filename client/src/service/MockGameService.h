// MockGameService.h
#pragma once

#include "GameService.h"
#include "ClientEventBus.h"
#include "base/ThreadPool.h"

#include <iostream>

namespace boost
{
  namespace uuids
  {
    class random_generator;
  }
}

namespace bgg
{
  class MockGameService : public GameService
  {
    util::ThreadPool threadPool_;
    std::shared_ptr<boost::uuids::random_generator> uuidGenerator_;
    std::shared_ptr<ClientEventBus> eventBus_;
    std::vector<std::size_t> subscriptionIdList_;

  public:
    MockGameService(std::shared_ptr<ClientEventBus> eventBus);
    ~MockGameService();

  private:
    void emit(ServerMessage const &msg) noexcept override;

    void sendRequest(LoginRequest const &loginRqt) noexcept;

    void sendRequest(FindGameRequest const &findGameRqt) noexcept;

    void sendRequest(CancelMatchmakingRequest const &cancelMatchmakingRqt) noexcept;

    void sendRequest(CommitMoveRequest const &commitMoveRqt) noexcept;

    void sendRequest(ResignGameRequest const &resignGameRqt) noexcept;

    static void simulateNetworkLatencyAndFailure(
        int failurePercent = 0, int minDelay = 100, int maxDelay = 2000);
  };

} // namespace bgg
