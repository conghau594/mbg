// GameApp.h
#pragma once

#include <boost/assert.hpp>
#include <memory>

#include "display/GameDisplay.h"
#include "service/GameService.h"

namespace bgg // stands for: board games galore
{
  class GameDisplay;

  // ==========================================================================
  class GameApp final
  {
    std::shared_ptr<GameDisplay> gameDisplay_;
    std::shared_ptr<GameService> gameService_;

  public:
    inline GameApp(
        std::shared_ptr<GameDisplay> gameDisplay,
        std::shared_ptr<GameService> gameService) noexcept;

    inline void run();
  };

  // ==========================================================================
  GameApp::GameApp(
      std::shared_ptr<GameDisplay> gameDisplay,
      std::shared_ptr<GameService> gameService) noexcept
      : gameDisplay_(std::move(gameDisplay)),
        gameService_(std::move(gameService))
  {
    BOOST_ASSERT_MSG(gameDisplay_, "gameDisplay_ of GameApp cannot be null.");
    BOOST_ASSERT_MSG(gameService_, "gameService_ of GameApp cannot be null.");
  }

  void GameApp::run()
  {
    gameDisplay_->run();
  }
}