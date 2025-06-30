// SfGameDisplay.h
#pragma once

#include <list>
#include <memory>
#include <string>
#include <mutex>
#include <format>

#include "GameDisplay.h"
#include "service/ClientEventBus.h"

#ifdef _DEBUG
#include <iostream>
#endif

namespace sf
{
  class RenderWindow;
}
namespace bgg
{
  class SfGameDisplay final : public GameDisplay
  {
    std::shared_ptr<sf::RenderWindow> window_;
    std::shared_ptr<ClientEventBus> eventBus_;

    std::shared_ptr<GameScreen> currentScreen_;
    std::list<std::shared_ptr<GameScreen>> screenStack_;

    std::list<ServerMessage> serverMessages_;
    std::mutex messageMutex_;
    std::vector<std::size_t> subscriptionIDs_;

  public:
    SfGameDisplay(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<ClientEventBus> eventBus);
    ~SfGameDisplay() noexcept;

    void run() override;
    void send(ClientEvent const &request) noexcept override;

    void pushScreen(std::shared_ptr<GameScreen> newScreen) noexcept override;
    void popScreen() noexcept override;
    void changeScreen(std::shared_ptr<GameScreen> newScreen) noexcept override;

  private:
    void subscribeServerMessages();
    void unsubscribeServerMessages();

    template <typename DATA>
      requires(peeb::is_in_template_v<DATA, ServerMessage>)
    void subscribe()
    {
      std::optional<std::size_t> id;
      id = eventBus_->subscribe<ServerMessage, DATA>(
          [this](DATA const &d)
          {
            std::lock_guard lock(messageMutex_);
            serverMessages_.push_back(d);
#ifdef _DEBUG
            std::clog << "\nSfGameDisplay has listened ServerMessage of "
                      << typeid(DATA).name();
#endif
          });
      if (!id)
      {
        std::string msg = std::format(
            "Cannot subscribe to `{}` from SfGameDisplay",
            typeid(DATA).name());
        throw(std::runtime_error(msg));
      }
      subscriptionIDs_.emplace_back(id.value());
#ifdef _DEBUG
      std::clog << "\nSfGameDisplay has subscribed to ServerMessage of "
                << typeid(DATA).name() << " successfully";
#endif
    }
  };

} // namespace bgg
