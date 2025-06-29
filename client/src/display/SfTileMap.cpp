// SfTileMap.cpp

#include <SFML/Graphics/RenderTarget.hpp>

#include "SfTileMap.h"

namespace bgg
{
  SfTileMap::SfTileMap(
      std::filesystem::path const &tileSetPath,
      sf::Vector2u const &tileSizeInPixels,
      unsigned const *const tileLevels,
      sf::Vector2u const &mapSizeInTiles)
  {
    // load the tileset texture
    if (!tileSet_.loadFromFile(tileSetPath))
    {
      throw std::runtime_error("Failed to load tile set texture from " + tileSetPath.string());
    }

    // check if the tile levels are valid
    if (tileLevels == nullptr)
    {
      throw std::invalid_argument("Invalid tile levels");
    }

    // resize the vertex array to fit the level size
    vertices_.setPrimitiveType(sf::PrimitiveType::Triangles);
    const unsigned tilesPerMapRow = mapSizeInTiles.x;
    const unsigned tilesPerMapCol = mapSizeInTiles.y;
    vertices_.resize(tilesPerMapRow * tilesPerMapCol * 6);

    // populate the vertex array, with two triangles per tile
    const unsigned tilesPerTextureWidth = tileSet_.getSize().x / tileSizeInPixels.x;
    for (unsigned i = 0; i < tilesPerMapRow; ++i)
    {
      for (unsigned j = 0; j < tilesPerMapCol; ++j)
      {
        // get the current tile number
        const unsigned tileNumber = tileLevels[i + j * tilesPerMapRow];

        // get a pointer to the triangles' vertices of the current tile
        sf::Vertex *triangles = &vertices_[(i + j * tilesPerMapRow) * 6];

        // define the 6 corners of the two triangles
        float tileWidth = float(tileSizeInPixels.x);
        float tileHeight = float(tileSizeInPixels.y);

        triangles[0].position = sf::Vector2f(tileWidth * i, tileHeight * j);
        triangles[1].position = triangles[0].position + sf::Vector2f(tileWidth, 0.0f);
        triangles[2].position = triangles[0].position + sf::Vector2f(0.0f, tileHeight);
        triangles[3].position = triangles[0].position + sf::Vector2f(tileWidth, tileHeight);
        triangles[4].position = triangles[1].position;
        triangles[5].position = triangles[2].position;

        // find its position in the tileset texture
        const sf::Vector2u tileTexCoords{
            tileNumber % tilesPerTextureWidth, tileNumber / tilesPerTextureWidth};

        // define the 6 matching texture coordinates
        triangles[0].texCoords = sf::Vector2f(float(tileTexCoords.x * tileSizeInPixels.x), float(tileTexCoords.y * tileSizeInPixels.y));
        triangles[1].texCoords = triangles[0].texCoords + sf::Vector2f(float(tileSizeInPixels.x), 0.0f);
        triangles[2].texCoords = triangles[0].texCoords + sf::Vector2f(0.0f, float(tileSizeInPixels.y));
        triangles[3].texCoords = triangles[0].texCoords + sf::Vector2f(tileSizeInPixels);
        triangles[4].texCoords = triangles[1].texCoords;
        triangles[5].texCoords = triangles[2].texCoords;
      }
    }

    tileSizeInPixels_ = tileSizeInPixels;
    mapSizeInPixels_ = sf::Vector2u{
        tileSizeInPixels.x * tilesPerMapRow, tileSizeInPixels.y * tilesPerMapCol};
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
    int tileX = int((x - getPosition().x) / tileSizeInPixels_.x);
    int tileY = int((y - getPosition().y) / tileSizeInPixels_.y);

    return sf::Vector2i(tileX, tileY);
  }

  void SfTileMap::fitRectangle(
      sf::Vector2u const &mapRegionTopLeft, sf::Vector2u const &mapRegionBotRight)
  {
    sf::Vector2u possibleMapRegionSize = mapRegionBotRight - mapRegionTopLeft;

    float widthRatio = float(possibleMapRegionSize.x) / mapSizeInPixels_.x;
    float heightRatio = float(possibleMapRegionSize.y) / mapSizeInPixels_.y;

    float minRatio = std::min(widthRatio, heightRatio);

    float newXPos = mapRegionTopLeft.x + (possibleMapRegionSize.x - minRatio * mapSizeInPixels_.x) * 0.5f;
    float newYPos = mapRegionTopLeft.y + (possibleMapRegionSize.y - minRatio * mapSizeInPixels_.y) * 0.5f;
    setPosition({newXPos, newYPos});

    setScale({minRatio, minRatio});

    mapSizeInPixels_.x = int(mapSizeInPixels_.x * minRatio);
    mapSizeInPixels_.y = int(mapSizeInPixels_.y * minRatio);

    tileSizeInPixels_.x = int(tileSizeInPixels_.x * minRatio);
    tileSizeInPixels_.y = int(tileSizeInPixels_.y * minRatio);
  }
}
