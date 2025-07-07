// SfChessTextureAtlas001.h
#pragma once

#include "SfTextureAtlas.h"
#include "ChessTextureCell.h"

namespace bgg
{
  class SfChessTextureAtlas001 final : public SfTextureAtlas
  {
    sf::Vector2i atlasCellSize_;
    sf::Vector2i atlasSizeInCells_;

  public:
    SfChessTextureAtlas001() noexcept
        : SfTextureAtlas(sf::Texture("resource/001-chess-texture-atlas.png")),
          atlasCellSize_{200, 200},
          atlasSizeInCells_{20, 1}
    {
    }

  private:
    [[nodiscard]]
    auto getRegion(int regionIndex) const -> sf::IntRect override
    {
      int scalePercent = 100;
      if (regionIndex == ChessTextureCell::CHOICE_HIGHLIGHTER)
      {
        scalePercent = 95;
      }
      else if (ChessTextureCell::WHITE_KING <= regionIndex &&
               regionIndex <= ChessTextureCell::BLACK_PAWN)
      {
        scalePercent = 105;
      }

      sf::Vector2i cellSize = (scalePercent * atlasCellSize_) / 100;
      sf::Vector2i moveVector = (atlasCellSize_ - cellSize) / 2;
      sf::Vector2i topleft = regionIndexToTexCoords(regionIndex) + moveVector;
      return {topleft, cellSize};
    }

    auto regionIndexToTexCoords(int regionIndex) const -> sf::Vector2i
    {
      int xCell = regionIndex % atlasSizeInCells_.x;
      int yCell = regionIndex / atlasSizeInCells_.x;
      return sf::Vector2i{atlasCellSize_.x * xCell, atlasCellSize_.y * yCell};
    }
  };
} // namespace bgg
