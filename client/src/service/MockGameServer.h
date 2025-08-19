// MockGameServer.h
#pragma once

#include "IGameServer.h"
#include "ClientEventBus.h"
#include "base/ThreadPool.h"

namespace boost
{
  namespace uuids
  {
    class random_generator;
  }
}

namespace bgg
{
  class MockGameServer : public IGameServer
  {
    utils::ThreadPool threadPool_;
    std::shared_ptr<boost::uuids::random_generator> uuidGenerator_;
    std::shared_ptr<ClientEventBus> eventBus_;
    std::vector<std::size_t> subscriptionIdList_;

  public:
    MockGameServer(std::shared_ptr<ClientEventBus> eventBus);
    ~MockGameServer();

  private:
    void emit(ServerMessage const &msg) noexcept override;

    void sendRequest(LoginRequest const &loginRqt) noexcept;

    void sendRequest(FindGameRequest const &findGameRqt) noexcept;

    void sendRequest(CancelMatchmakingRequest const &cancelMatchmakingRqt) noexcept;

    void sendRequest(MoveRequest const &commitMoveRqt) noexcept;

    void sendRequest(ResignGameRequest const &resignGameRqt) noexcept;

    static void simulateNetworkLatencyAndFailure(
        int failurePercent = 0, int minDelay = 100, int maxDelay = 2000);
  };

} // namespace bgg
