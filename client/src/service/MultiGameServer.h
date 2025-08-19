// MultiGameServer.h
#pragma once

#include "IGameServer.h"
#include "ClientEventBus.h"
#include "IGameService.h"

#include "base/ThreadPool.h"

namespace bgg
{
  class MultiGameServer : public IGameServer
  {
    utils::ThreadPool threadPool_;
    std::shared_ptr<ClientEventBus> eventBus_;
    std::vector<std::size_t> subscriptionIdList_;
    std::shared_ptr<IGameService> currentService_;

  public:
    MultiGameServer(std::shared_ptr<ClientEventBus> eventBus);
    ~MultiGameServer() noexcept;

    void emit(ServerMessage const &msg) noexcept override;

    void handleFindGameRequest(FindGameRequest const &findGameRqt) noexcept;
  };

} // namespace bgg
