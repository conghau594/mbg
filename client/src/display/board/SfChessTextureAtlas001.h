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
      sf::Vector2i topleft{atlasCellSize_.x * regionIndex, 0};
      return {topleft, atlasCellSize_};
    }
  };
} // namespace bgg
