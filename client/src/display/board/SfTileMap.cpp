// SfTileMap.cpp

#include <boost/assert.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "SfTileMap.h"
#include "SfTextureAtlas.h"

namespace bgg
{
  SfTileMap::SfTileMap(SfTextureAtlas const *atlas,
                       sf::Vector2i mapSizeInTiles,
                       std::vector<sf::Vector2i> const &tileLayout)
      : atlas_(atlas),
        mapSizeInTiles_(std::move(mapSizeInTiles))
  {
    // check if the tile levels are valid
    BOOST_ASSERT_MSG(tileLayout.size() >= mapSizeInTiles.x * mapSizeInTiles.y,
                     "Invalid tile layout");

    // resize the vertex array to fit the level size
    vertices_.setPrimitiveType(sf::PrimitiveType::Triangles);
    int const tilesPerMapRow = mapSizeInTiles.x;
    int const tilesPerMapCol = mapSizeInTiles.y;
    vertices_.resize(6 * tilesPerMapRow * tilesPerMapCol);

    float const tileWidth = float(atlas->cellSize.x);
    float const tileHeight = float(atlas->cellSize.y);

    // populate the vertex array, with two triangles per tile
    for (int i = 0; i < tilesPerMapRow; ++i)
    {
      for (int j = 0; j < tilesPerMapCol; ++j)
      {
        // get a pointer to the triangles' vertices of the current tile
        sf::Vertex *triangles = &vertices_[(i + j * tilesPerMapRow) * 6];

        // define the 6 corners of the two triangles
        //
        //       0   3----5
        //      |  \  \   |
        //      |   \  \  |
        //      2----1   4
        //
        triangles[0].position = sf::Vector2f(float(i) * tileWidth, float(j) * tileHeight);
        triangles[1].position = triangles[0].position + sf::Vector2f(atlas->cellSize);
        triangles[2].position = triangles[0].position + sf::Vector2f(0.0f, tileHeight);
        triangles[3].position = triangles[0].position;
        triangles[4].position = triangles[1].position;
        triangles[5].position = triangles[0].position + sf::Vector2f(tileWidth, 0.0f);

        // get its position in the tileset texture
        sf::Vector2f const tileTexCoords = sf::Vector2f(tileLayout[i + j * tilesPerMapRow]);

        // define the 6 matching texture coordinates

        triangles[0].texCoords = tileTexCoords.componentWiseMul(sf::Vector2f(atlas->cellSize));
        triangles[1].texCoords = triangles[0].texCoords + sf::Vector2f(atlas->cellSize);
        triangles[2].texCoords = triangles[0].texCoords + sf::Vector2f(0.0f, tileHeight);
        triangles[3].texCoords = triangles[0].texCoords;
        triangles[4].texCoords = triangles[1].texCoords;
        triangles[5].texCoords = triangles[0].texCoords + sf::Vector2f(tileWidth, 0.0f);
      }
    }
  }

  void SfTileMap::draw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = &(atlas_->texture);

    // draw the vertex array
    target.draw(vertices_, states);
  }

  auto SfTileMap::getMapSizeInTiles() const noexcept -> sf::Vector2i const &
  {
    return mapSizeInTiles_;
  }
}
