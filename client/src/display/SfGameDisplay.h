// SfGameDisplay.h
#pragma once

#include <list>
#include <memory>
#include <string>

#include "GameDisplay.h"
#include "service/ClientEventBus.h"

namespace sf
{
  class RenderWindow;
}
namespace bgg
{
  class SfGameDisplay final : public GameDisplay
  {
    std::shared_ptr<sf::RenderWindow> window_;
    std::list<std::shared_ptr<GameScreen>> screenStack_;
    std::shared_ptr<GameScreen> currentScreen_;
    std::shared_ptr<ClientEventBus> eventBus_;

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
  };

} // namespace bgg
