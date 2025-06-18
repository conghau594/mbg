// SfGameSelectionScreen.h
#pragma once

#include "SfBaseScreen.h"

namespace iac
{
  class SfGameSelectionScreen final : public SfBaseScreen
  {
  public:
    SfGameSelectionScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameDisplay> displayContext,
        size_t updatePeriod) noexcept;

    void onExit() noexcept override;
    void onEnter() noexcept override;

  private:
    void onPeriodicUpdate() noexcept override;
    void onEvent(std::optional<sf::Event> event) noexcept override;
    void drawMenu() noexcept;
    void render() noexcept override;
  };

} // namespace iac
