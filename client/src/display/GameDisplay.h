// GameDisplay.h
#pragma once

#include <list>
#include <memory>
#include <string>
#include <mutex>
#include <format>

#include "IDisplay.h"
#include "service/ClientEventBus.h"

#include "base/Logger.h"

namespace sf
{
  class RenderWindow;
}
namespace bgg
{
  class GameDisplay final : public IDisplay
  {
    std::shared_ptr<sf::RenderWindow> window_;
    std::shared_ptr<ClientEventBus> eventBus_;

    std::shared_ptr<IScreen> currentScreen_;
    std::list<std::shared_ptr<IScreen>> screenStack_;

    std::list<ServerMessage> serverMessages_;
    std::mutex messageMutex_;
    std::vector<std::size_t> subscriptionIDs_;

  public:
    GameDisplay(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<ClientEventBus> eventBus);
    ~GameDisplay() noexcept;

  private:
    void run() override;
    void send(ClientRequest const &request) noexcept override;

    void pushScreen(std::shared_ptr<IScreen> newScreen) noexcept override;
    void popScreen() noexcept override;
    void changeScreen(std::shared_ptr<IScreen> newScreen) noexcept override;

    void subscribeServerMessages();
    void unsubscribeServerMessages();

    template <typename DATA>
      requires(peeb::is_in_template_v<DATA, ServerMessage>)
    inline void subscribe();
  };

  template <typename DATA>
    requires(peeb::is_in_template_v<DATA, ServerMessage>)
  void GameDisplay::subscribe()
  {
    std::optional<std::size_t> id = eventBus_->subscribe<ServerMessage, DATA>(
        [this](DATA const &d)
        {
          std::lock_guard lock(messageMutex_);
          serverMessages_.push_back(d);

          SPDLOG_INFO("A server message of type '{}' has come to '{}'",
                      typeid(DATA).name(), typeid(*this).name());
        });

    if (!id)
    {
      std::string msg = std::format(
          "Cannot subscribe to '{}' from '{}'",
          typeid(DATA).name(), typeid(*this).name());
      throw(std::runtime_error(msg));
    }
    subscriptionIDs_.emplace_back(id.value());

    SPDLOG_INFO("'{}' has subscribed to server messages of type '{}'",
                typeid(*this).name(), typeid(DATA).name());
  }
} // namespace bgg
