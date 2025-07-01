// GameBoardScreenFactory.h
#pragma once

#include "GameScreen.h"
#include "GameDisplay.h"
#include "SfChessScreen.h"
#include "model/GameType.h"

namespace bgg
{
  class GameBoardScreenFactory final
  {
  public:
    auto create(
        int type,
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameDisplay> gameDisplay) const noexcept
        -> std::shared_ptr<GameScreen>
    {
      switch (type)
      {
      case GameType::WESTERN_CHESS:
        return createWesternChessScreen(window, gameDisplay);
      case GameType::GOMOKU:
      default:
        return nullptr;
      }
    }

  private:
    auto createWesternChessScreen(
        std::shared_ptr<sf::RenderWindow> window,
        std::shared_ptr<GameDisplay> gameDisplay) const noexcept
        -> std::shared_ptr<GameScreen>
    {
      constexpr unsigned const level[] = {
          0, 1, 0, 1, 0, 1, 0, 1,
          1, 0, 1, 0, 1, 0, 1, 0,
          0, 1, 0, 1, 0, 1, 0, 1,
          1, 0, 1, 0, 1, 0, 1, 0,
          0, 1, 0, 1, 0, 1, 0, 1,
          1, 0, 1, 0, 1, 0, 1, 0,
          0, 1, 0, 1, 0, 1, 0, 1,
          1, 0, 1, 0, 1, 0, 1, 0};

      SfTileMap tileMap(
          "resource/western-chess-tile-set.png",
          {200, 200},
          level,
          {8, 8});

      std::shared_ptr<GameScreen> chessScreen = std::make_shared<SfChessScreen>(
          window, gameDisplay, tileMap);
      return chessScreen;
    }
  };

} // namespace bgg
