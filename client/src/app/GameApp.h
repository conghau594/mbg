// GameApp.h
#pragma once

#include <boost/assert.hpp>
#include <memory>

#include "connect/ServerConnector.h"
#include "display/GameDisplay.h"

namespace iac
{
  class GameDisplay;
  class ServerConnector;

  class GameApp final
  {
    std::shared_ptr<ServerConnector> serverConnector_;
    std::shared_ptr<GameDisplay> gameDisplay_;

  public:
    GameApp(
        std::shared_ptr<GameDisplay> gameDisplay,
        std::shared_ptr<ServerConnector> serverConnector) noexcept;

    inline void run();
  };

  // ==========================================================================
  GameApp::GameApp(
      std::shared_ptr<GameDisplay> gameDisplay,
      std::shared_ptr<ServerConnector> serverConnector) noexcept
      : gameDisplay_(gameDisplay), serverConnector_(serverConnector)
  {
    BOOST_ASSERT_MSG(
        gameDisplay, "`gameDisplay` of `GameApp` cannot be null.");
    BOOST_ASSERT_MSG(
        serverConnector, "`serverConnector` of `GameApp` cannot be null.");
  }

  void GameApp::run()
  {
    serverConnector_->run();
    gameDisplay_->run();
  }
}