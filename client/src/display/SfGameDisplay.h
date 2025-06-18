// SfGameDisplay.h
#pragma once

#include <list>
#include <memory>
#include <string>
#include "GameDisplay.h"

namespace sf
{
  class RenderWindow;
}

namespace iab
{
  class SfGameDisplay final : public GameDisplay
  {
    std::shared_ptr<sf::RenderWindow> window_;
    std::list<std::shared_ptr<GameScreen>> screenStack_;
    std::shared_ptr<GameScreen> lastScreen_;

  public:
    SfGameDisplay(std::shared_ptr<sf::RenderWindow> window);
    ~SfGameDisplay() noexcept;

    void run() override;
    void pushScreen(std::shared_ptr<GameScreen> newScreen) noexcept override;
    void popScreen() noexcept override;
    void changeScreen(std::shared_ptr<GameScreen> newScreen) noexcept override;
  };

} // namespace iab
