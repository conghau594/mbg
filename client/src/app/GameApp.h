// GameApp.h
#pragma once

#include <boost/assert.hpp>
#include <memory>

#include "display/GameDisplay.h"

namespace bgg // stands for: board games galore
{
  class GameDisplay;

  // ==========================================================================
  class GameApp final
  {
    std::shared_ptr<GameDisplay> gameDisplay_;

  public:
    inline GameApp(std::shared_ptr<GameDisplay> gameDisplay) noexcept;

    inline void run();
  };

  // ==========================================================================
  GameApp::GameApp(
      std::shared_ptr<GameDisplay> gameDisplay) noexcept
      : gameDisplay_(std::move(gameDisplay))
  {
    BOOST_ASSERT_MSG(gameDisplay_, "gameDisplay_ of GameApp cannot be null.");
  }

  void GameApp::run()
  {
    gameDisplay_->run();
  }
}