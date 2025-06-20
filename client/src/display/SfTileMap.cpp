// SfTileMap.cpp

#include <SFML/Graphics/RenderTarget.hpp>

#include "SfTileMap.h"

namespace iab
{
  auto SfTileMap::load(
      std::filesystem::path const &tileSetPath,
      sf::Vector2i const &startPoint,
      sf::Vector2u const &tileSize,
      int const *const tileLevels,
      int const tilesPerRow,
      int const tilesPerCol) noexcept -> bool
  {
    // load the tileset texture
    if (!tileSet_.loadFromFile(tileSetPath))
      return false;

    startPoint_ = startPoint;
    tileSize_ = tileSize;

    // resize the vertex array to fit the level size
    vertices_.setPrimitiveType(sf::PrimitiveType::TriangleStrip);
    vertices_.resize(tilesPerRow * tilesPerCol * 6);

    // populate the vertex array, with two triangles per tile
    const int tilesPerTextureWidth = tileSet_.getSize().x / tileSize.x;
    for (int i = 0; i < tilesPerRow; ++i)
    {
      for (int j = 0; j < tilesPerCol; ++j)
      {
        // get the current tile number
        const int tileNumber = tileLevels[i + j * tilesPerRow];

        // get a pointer to the triangles' vertices of the current tile
        sf::Vertex *triangles = &vertices_[(i + j * tilesPerRow) * 6];

        // define the 6 corners of the two triangles
        triangles[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
        triangles[1].position = triangles[0].position + sf::Vector2f(tileSize.x, 0);
        triangles[2].position = triangles[0].position + sf::Vector2f(0, tileSize.y);
        triangles[3].position = triangles[2].position;
        triangles[4].position = triangles[0].position + sf::Vector2f(tileSize.x, 0);
        triangles[5].position = triangles[4].position + sf::Vector2f(0, tileSize.y);

        // find its position in the tileset texture
        const int tileXPos = tileNumber % tilesPerTextureWidth;
        const int tileYPos = tileNumber / tilesPerTextureWidth;

        // define the 6 matching texture coordinates
        triangles[0].texCoords = sf::Vector2f(tileXPos * tileSize.x, tileYPos * tileSize.y);
        triangles[1].texCoords = triangles[0].texCoords + sf::Vector2f(tileSize.x, 0);
        triangles[2].texCoords = triangles[0].texCoords + sf::Vector2f(0, tileSize.y);
        triangles[3].texCoords = triangles[2].texCoords;
        triangles[4].texCoords = triangles[0].texCoords + sf::Vector2f(tileSize.x, 0);
        triangles[5].texCoords = triangles[4].texCoords + sf::Vector2f(0, tileSize.y);
      }
    }

    return true;
  }

  void SfTileMap::draw(sf::RenderTarget &target, sf::RenderStates states) const noexcept
  {
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = &tileSet_;

    // draw the vertex array
    target.draw(vertices_, states);
  }

  auto SfTileMap::getTileCoords(int x, int y) const noexcept -> sf::Vector2i
  {
    int tileX = (x - startPoint_.x) / tileSize_.x;
    int tileY = (y - startPoint_.y) / tileSize_.y;

    return sf::Vector2i(tileX, tileY);
  }
}
