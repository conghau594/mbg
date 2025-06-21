// SfTileMap.cpp

#include <SFML/Graphics/RenderTarget.hpp>

#include "SfTileMap.h"

namespace iab
{
  SfTileMap::SfTileMap(
      std::filesystem::path const &tileSetPath,
      sf::Vector2i const &startPoint,
      sf::Vector2u const &tileSize,
      int const *const tileLevels,
      int const tilesPerRow,
      int const tilesPerCol)
  {
    // load the tileset texture
    if (!tileSet_.loadFromFile(tileSetPath))
    {
      throw std::runtime_error("Failed to load tile set texture from " + tileSetPath.string());
    }
    // check if the tile levels are valid
    if (tilesPerRow <= 0 || tilesPerCol <= 0 || tileLevels == nullptr)
    {
      throw std::invalid_argument("Invalid tile levels or dimensions");
    }

    startPoint_ = startPoint;
    tileSize_ = tileSize;

    // resize the vertex array to fit the level size
    vertices_.setPrimitiveType(sf::PrimitiveType::Triangles);
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
        float displayTileWidth = 0.2f * tileSize.x;
        float displayTileHeight = 0.2f * tileSize.y;

        triangles[0].position = sf::Vector2f(float(i * displayTileWidth), float(j * displayTileHeight));
        triangles[1].position = triangles[0].position + sf::Vector2f(float(displayTileWidth), 0.0f);
        triangles[2].position = triangles[0].position + sf::Vector2f(0.0f, float(displayTileHeight));
        triangles[3].position = triangles[0].position + sf::Vector2f(float(displayTileWidth), float(displayTileHeight));
        triangles[4].position = triangles[1].position;
        triangles[5].position = triangles[2].position;

        // find its position in the tileset texture
        const int tileXPos = tileNumber % tilesPerTextureWidth;
        const int tileYPos = tileNumber / tilesPerTextureWidth;

        // define the 6 matching texture coordinates
        triangles[0].texCoords = sf::Vector2f(float(tileXPos * tileSize.x), float(tileYPos * tileSize.y));
        triangles[1].texCoords = triangles[0].texCoords + sf::Vector2f(float(tileSize.x), 0.0f);
        triangles[2].texCoords = triangles[0].texCoords + sf::Vector2f(0.0f, float(tileSize.y));
        triangles[3].texCoords = triangles[0].texCoords + sf::Vector2f(float(tileSize.x), float(tileSize.y));
        triangles[4].texCoords = triangles[1].texCoords;
        triangles[5].texCoords = triangles[2].texCoords;
      }
    }
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
