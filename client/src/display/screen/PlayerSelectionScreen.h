// PlayerSelectionScreen.h
#pragma once

#include <future>

#include <SFML/System/Time.hpp>

#include "BaseScreen.h"
#include "MessageScreen.h"
#include "service/ServerMessage.h"

struct ImFont;
namespace bgg
{
  class PlayerSelectionScreen final : public BaseScreen
  {
    std::shared_ptr<IDisplay> gameDisplay_;
    std::vector<std::string> playerTypeNames_;
    int pressedButtonIndex_;
    int gameType_;

  public:
    PlayerSelectionScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<IDisplay> gameDisplay_,
        std::vector<std::string> playerTypeNames,
        int gameType) noexcept;

  private:
    void update(sf::Time const &elapsed) noexcept override;

    void onWindowEventExceptClosed(std::optional<sf::Event> const &event) noexcept override;

    void doExit() noexcept override;
    void doEnter() noexcept override;

    void layOutScreen() noexcept;
  };

} // namespace bgg
