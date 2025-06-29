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
  class SfMatchmakingCancelScreen final : public SfMessageScreen
  {
    std::future<ServerMessage> const &futureCancelResponse_;
    std::shared_ptr<GameScreen> parentScreen_;

  public:
    SfMatchmakingCancelScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameScreen> parentScreen,
        std::future<ServerMessage> const &futureCancelResponse_);

  private:
    void update(sf::Time const &elapsed) noexcept override;
  };

  /////////////////////////////////////////////////////////////////////////////
  class SfGameFindingScreen final : public SfMessageScreen
  {
    std::shared_ptr<GameDisplay> gameDisplay_;

    std::future<ServerMessage> futureFindGameResponse_;
    std::future<ServerMessage> futureCancelResponse_;
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
  };
}