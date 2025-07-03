// GameBoardFactory.h
#pragma once

#include "model/GameType.h"
#include "SfTileMap.h"
#include "SfChessBoard.h"

namespace bgg
{
  class GameBoardFactory final
  {
  public:
    auto create(int type) const
        -> std::shared_ptr<SfGameBoard>
    {
      switch (type)
      {
      case GameType::CHESS:
        return createChessBoard();
      case GameType::GOMOKU:
      default:
        return nullptr;
      }
    }

  private:
    auto createChessBoard() const -> std::shared_ptr<SfGameBoard>
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

      std::shared_ptr<SfGameBoard> chessBoard = std::make_shared<SfChessBoard>(/**/);
      return chessBoard;
    }
  };

} // namespace bgg
