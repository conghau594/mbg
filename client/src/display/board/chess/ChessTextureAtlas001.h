// ChessTextureAtlas001.h
#pragma once

#include "display/board/BaseTextureAtlas.h"
#include "ChessTextureCell.h"

namespace bgg
{
  class ChessTextureAtlas001 final : public BaseTextureAtlas
  {
    sf::Vector2i atlasCellSize_;
    sf::Vector2i atlasSizeInCells_;

  public:
    ChessTextureAtlas001() noexcept
        : BaseTextureAtlas(sf::Texture("resource/001-chess-texture-atlas.png")),
          atlasCellSize_{200, 200},
          atlasSizeInCells_{20, 1}
    {
    }

  private:
    auto getRegion(int regionIndex) const -> sf::IntRect override
    {
      sf::Vector2i scalePercents = {100, 100};
      if (regionIndex == int(ChessTextureCell::CHOICE_HIGHLIGHTER))
      {
        scalePercents = {95, 95};
      }
      else if (regionIndex == int(ChessTextureCell::CHECK_HIGHLIGHTER))
      {
        scalePercents = {80, 80};
      }
      else if (int(ChessTextureCell::WHITE_KING) <= regionIndex &&
               regionIndex <= int(ChessTextureCell::BLACK_PAWN))
      {
        scalePercents = {100, 100};
      }

      sf::Vector2i cellSize = scalePercents.componentWiseMul(atlasCellSize_) / 100;
      sf::Vector2i moveVector = (atlasCellSize_ - cellSize) / 2;
      sf::Vector2i topleft = regionIndexToTexCoords(regionIndex) + moveVector;
      return {topleft, cellSize};
    }

    [[nodiscard]]
    auto regionIndexToTexCoords(int regionIndex) const -> sf::Vector2i
    {
      int xCell = regionIndex % atlasSizeInCells_.x;
      int yCell = regionIndex / atlasSizeInCells_.x;
      return sf::Vector2i{atlasCellSize_.x * xCell, atlasCellSize_.y * yCell};
    }
  };
} // namespace bgg
