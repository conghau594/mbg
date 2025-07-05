// SfTextureAtlas.h
#pragma once

namespace bgg
{
  class SfTextureAtlas final
  {
  public:
    sf::Texture texture;
    sf::Vector2i cellSize;
    int cellsPerRow;
    int cellsPerCol;
  };

} // namespace bgg
