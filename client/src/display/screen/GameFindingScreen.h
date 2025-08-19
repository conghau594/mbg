// GameFindingScreen.h
#pragma once

#include <future>

#include <SFML/System/Time.hpp>

#include "BaseScreen.h"
#include "MessageScreen.h"
#include "service/ServerMessage.h"

struct ImFont;
namespace bgg
{
  class GameFindingScreen final : public MessageScreen
  {
    std::shared_ptr<IDisplay> gameDisplay_;
    bool isCancelButtonPressed_;

  public:
    GameFindingScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<IDisplay> gameDisplay);

  private:
    void update(sf::Time const &elapsed) noexcept override;
    void doEnter() override;
    void doExit() override;

    void sendCancelMatchmakingRequest() noexcept;

    void onCancelMatchmakingResponse(CancelMatchmakingResponse const &response) noexcept;
    void onFindGameResponse(FindGameResponse const &response) noexcept;
  };
}