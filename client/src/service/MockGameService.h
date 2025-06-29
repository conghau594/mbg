// MockGameService.h
#pragma once

#include "GameService.h"
#include "ClientRequest.h"
#include "ServerMessage.h"
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
  class MockGameService : public GameService
  {
    util::ThreadPool threadPool_;
    std::shared_ptr<boost::uuids::random_generator> uuidGenerator_;

  public:
    MockGameService() noexcept;
    void send(ServerMessage const &msg) noexcept override;

    // std::future<ServerMessage> operator()(
    //     ClientRequest::Login const &loginRqt) noexcept;

    // std::future<ServerMessage> operator()(
    //     ClientRequest::FindGame const &findGameRqt) noexcept;

    // std::future<ServerMessage> operator()(
    //     ClientRequest::CancelMatchmaking const &cancelMatchmakingRqt) noexcept;

    // std::future<ServerMessage> operator()(
    //     ClientRequest::CommitMove const &commitMoveRqt) noexcept;

    // std::future<ServerMessage> operator()(
    //     ClientRequest::ResignGame const &resignGameRqt) noexcept;

    static void simulateNetworkLatencyAndFailure(
        int failurePercent = 0, int minDelay = 100, int maxDelay = 2000);
  };

} // namespace bgg
