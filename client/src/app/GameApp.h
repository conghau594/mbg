// GameApp.h
#pragma once

#include <boost/assert.hpp>
#include <memory>

#include "connect/GameService.h"
#include "display/GameDisplay.h"

namespace iab
{
  class GameDisplay;
  class ServerConnector;

  // ==========================================================================
  class GameApp final
  {
    std::shared_ptr<GameService> gameService_;
    std::shared_ptr<GameDisplay> gameDisplay_;

  public:
    GameApp(
        std::shared_ptr<GameDisplay> gameDisplay,
        std::shared_ptr<GameService> gameService) noexcept;

    inline void run();
  };

  // ==========================================================================
  GameApp::GameApp(
      std::shared_ptr<GameDisplay> gameDisplay,
      std::shared_ptr<GameService> gameService) noexcept
      : gameDisplay_(gameDisplay), gameService_(gameService)
  {
    BOOST_ASSERT_MSG(
        gameDisplay, "`gameDisplay` of `GameApp` cannot be null.");
    BOOST_ASSERT_MSG(
        gameService, "`serverConnector` of `GameApp` cannot be null.");
  }

  void GameApp::run()
  {
    // serverConnector_->run();
    gameDisplay_->run();
  }
}