// SfTileMap.cpp

#include <boost/assert.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "SfTileMap.h"
#include "SfTextureAtlas.h"

namespace bgg
{
  SfTileMap::SfTileMap(std::shared_ptr<const SfTextureAtlas> mapTextureAtlas,
                       sf::Vector2i mapSizeInTiles,
                       std::vector<int> const &tileLayout)
      : mapTextureAtlas_(std::move(mapTextureAtlas)),
        mapSizeInTiles_(std::move(mapSizeInTiles))
  {
    // check if the tile levels are valid
    BOOST_ASSERT_MSG(0 < tileLayout.size() &&
                         tileLayout.size() >= mapSizeInTiles.x * mapSizeInTiles.y,
                     "Invalid tile layout");

    // resize the vertex array to fit the level size
    int const tilesPerMapRow = mapSizeInTiles.x;
    int const tilesPerMapCol = mapSizeInTiles.y;
    vertices_.setPrimitiveType(sf::PrimitiveType::Triangles);
    vertices_.resize(6 * tilesPerMapRow * tilesPerMapCol);

    // Get the region of the first tile and make it as the base rectangle
    sf::FloatRect const firstRegionRect = sf::FloatRect(mapTextureAtlas_->getRegion(tileLayout[0]));
    float const &firstTileWidth = firstRegionRect.size.x;
    float const &firstTileHeight = firstRegionRect.size.y;

    // populate the vertex array, with two triangles per tile
    for (int i = 0; i < tilesPerMapRow; ++i)
    {
      for (int j = 0; j < tilesPerMapCol; ++j)
      {
        // get a pointer to the triangles' vertices of the current tile
        sf::Vertex *triangles = &vertices_[(i + j * tilesPerMapRow) * 6];
        // get its position in the tileset texture
        int const &curRegionIndex = tileLayout[i + j * tilesPerMapRow];
        sf::FloatRect const curRegionRect = sf::FloatRect(mapTextureAtlas_->getRegion(curRegionIndex));

        float const &curTileWidth = curRegionRect.size.x;
        float const &curTileHeight = curRegionRect.size.y;

        // define the 6 corners of the two triangles which compose a rectangle
        // with the same size as the above firstRegionRect
        //
        //      0 . 3-----4
        //      |   .     |
        //      |     .   |
        //      1-----2 . 5
        //
        triangles[0].position = sf::Vector2f(float(i) * firstTileWidth, float(j) * firstTileHeight);
        triangles[1].position = triangles[0].position + sf::Vector2f(0.0f, firstTileHeight);
        triangles[2].position = triangles[0].position + sf::Vector2f(firstTileWidth, firstTileHeight);
        triangles[3].position = triangles[0].position;
        triangles[4].position = triangles[0].position + sf::Vector2f(firstTileWidth, 0.0f);
        triangles[5].position = triangles[2].position;

        // define the 6 matching texture coordinates
        triangles[0].texCoords = sf::Vector2f(curRegionRect.position);
        triangles[1].texCoords = triangles[0].texCoords + sf::Vector2f(0.0f, curTileHeight);
        triangles[2].texCoords = triangles[0].texCoords + sf::Vector2f(curTileWidth, curTileHeight);
        triangles[3].texCoords = triangles[0].texCoords;
        triangles[4].texCoords = triangles[0].texCoords + sf::Vector2f(curTileWidth, 0.0f);
        triangles[5].texCoords = triangles[2].texCoords;
      }
    }
  }

  void SfTileMap::draw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    // apply the transform
    states.transform *= transformer_.getTransform();

    // apply the tileset texture
    states.texture = mapTextureAtlas_->getTexture();

    // draw the vertex array
    target.draw(vertices_, states);
  }

  void SfTileMap::setVisible(bool visible) noexcept
  {
    isVisible_ = visible;
  }

  void SfTileMap::setScale(sf::Vector2f const &factors) noexcept
  {
    transformer_.setScale(factors);
  }

  void SfTileMap::setPosition(sf::Vector2i const &position) noexcept
  {
    transformer_.setPosition(sf::Vector2f(position));
  }

  void SfTileMap::scale(sf::Vector2f const &factors) noexcept
  {
    transformer_.scale(factors);
  }

  void SfTileMap::move(sf::Vector2i const &offset) noexcept
  {
    transformer_.move(sf::Vector2f(offset));
  }

  auto SfTileMap::isVisible() const noexcept -> bool
  {
    return isVisible_;
  }

  auto SfTileMap::getSizeInTiles() const noexcept -> sf::Vector2i
  {
    return mapSizeInTiles_;
  }

  auto SfTileMap::getPosition() const noexcept -> sf::Vector2i
  {
    return sf::Vector2i(transformer_.getPosition());
  }

  auto SfTileMap::getOrigin() const noexcept -> sf::Vector2i
  {
    return sf::Vector2i(transformer_.getOrigin());
  }

  auto SfTileMap::getSize() const noexcept -> sf::Vector2i
  {

    return sf::Vector2i(vertices_.getBounds().size);
  }

  auto SfTileMap::getTileSize() const noexcept -> sf::Vector2i
  {
    //
    //      0 . 3-----4
    //      |   .     |
    //      |     .   |
    //      1-----2 . 5
    //
    sf::Vector2i tileSize = sf::Vector2i(transformer_.getScale().componentWiseMul(
        vertices_[5].position - vertices_[0].position));
    return tileSize;
  }

  auto SfTileMap::screenToTile(sf::Vector2i const &screenCoords) const noexcept
      -> sf::Vector2i
  {
    sf::Vector2i mapTopLeft = sf::Vector2i(transformer_.getTransform().transformPoint(
        vertices_[0].position));

    return (screenCoords - mapTopLeft).componentWiseDiv(getTileSize());
  }

  auto SfTileMap::tileToScreenRect(sf::Vector2i const &tileCoords) const noexcept
      -> sf::IntRect
  {
    sf::Vector2i mapTopLeft = sf::Vector2i(transformer_.getTransform().transformPoint(
        vertices_[0].position));
    sf::Vector2i tileSize = getTileSize();
    sf::Vector2i tileTopLeft = mapTopLeft + tileSize.componentWiseMul(tileCoords);
    return sf::IntRect(tileTopLeft, tileSize);
  }
}
