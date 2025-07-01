// SfGameFindingScreen.h
#pragma once

#include <future>

#include <SFML/System/Time.hpp>

#include "SfBaseScreen.h"
#include "SfMessageScreen.h"
#include "service/ServerMessage.h"

struct ImFont;
namespace bgg
{
  class SfGameFindingScreen final : public SfMessageScreen
  {
    std::shared_ptr<GameDisplay> gameDisplay_;
    int gameType_;
    int playerType_;
    bool isCancelButtonPressed_;

  public:
    SfGameFindingScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameDisplay> gameDisplay,
        int gameType,
        int playerType);

  private:
    void update(sf::Time const &elapsed) noexcept override;
    void goToGamePlayScreen() noexcept;

    void sendCancelMatchmakingRequest() noexcept;
    void onCancelMatchmakingResponse(CancelMatchmakingResponse const &response) noexcept;
    void onFindGameResponse(FindGameResponse const &response) noexcept;
  };
}