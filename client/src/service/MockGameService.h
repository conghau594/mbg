// MockGameService.h
#pragma once

#include "GameService.h"
#include "Request.h"
#include "Response.h"
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
  class MockGameService : public GameService
  {
    util::ThreadPool threadPool_;
    std::shared_ptr<boost::uuids::random_generator> uuidGenerator_;
    bool isConnected_;

  public:
    MockGameService() noexcept;
    std::future<Response> send(Request request) noexcept override;

    std::future<Response> operator()(
        Request::Login const &loginRqt) noexcept;

    std::future<Response> operator()(
        Request::FindGame const &findGameRqt) noexcept;

    std::future<Response> operator()(
        Request::CancelMatchmaking const &cancelMatchmakingRqt) noexcept;

    std::future<Response> operator()(
        Request::CommitMove const &commitMoveRqt) noexcept;

    std::future<Response> operator()(
        Request::ResignGame const &resignGameRqt) noexcept;

    static void simulateNetworkLatencyAndFailure(
        int failurePercent = 0, int minDelay = 100, int maxDelay = 2000);
  };

} // namespace iab
