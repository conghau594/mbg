// TileMap.cpp
#include "base/Logger.h"

#include <boost/assert.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "TileMap.h"
#include "BaseTextureAtlas.h"
#include "BoardItem.h"

namespace bgg
{
  TileMap::TileMap(std::shared_ptr<const BaseTextureAtlas> mapTextureAtlas,
                       sf::Vector2i mapSizeInTiles,
                       std::vector<int> const &tileLayout)
      : mapTextureAtlas_(std::move(mapTextureAtlas)),
        vertices_(),
        mapSizeInTiles_(std::move(mapSizeInTiles)),
        transformer_(),
        isVisible_(true)
  {
    // check if the tile levels are valid
    BOOST_ASSERT_MSG(0 < tileLayout.size() &&
                         tileLayout.size() <= std::size_t(mapSizeInTiles.x * mapSizeInTiles.y),
                     "Invalid tile layout");

    // resize the vertex array to fit the level size
    int const tilesPerMapRow = mapSizeInTiles.x;
    int const tilesPerMapCol = mapSizeInTiles.y;
    vertices_.setPrimitiveType(sf::PrimitiveType::Triangles);
    vertices_.resize(std::size_t(6 * tilesPerMapRow * tilesPerMapCol));

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
        sf::Vertex *triangles = &vertices_[std::size_t(6 * (i + j * tilesPerMapRow))];
        // get its position in the tileset texture
        int const &curRegionIndex = tileLayout[std::size_t(i + j * tilesPerMapRow)];
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

  void TileMap::draw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    // apply the transform
    states.transform *= transformer_.getTransform();

    // apply the tileset texture
    states.texture = mapTextureAtlas_->getTexture();

    // draw the vertex array
    target.draw(vertices_, states);
  }

  void TileMap::setVisible(bool visible) noexcept
  {
    isVisible_ = visible;
  }

  void TileMap::setScale(sf::Vector2f const &factors) noexcept
  {
    transformer_.setScale(factors);
  }

  void TileMap::setPosition(sf::Vector2i const &position) noexcept
  {
    transformer_.setPosition(sf::Vector2f(position));
  }

  void TileMap::setOrigin(sf::Vector2i const &origin) noexcept
  {
    transformer_.setOrigin(sf::Vector2f(origin));
  }

  void TileMap::scale(sf::Vector2f const &factors) noexcept
  {
    transformer_.scale(factors);
  }

  void TileMap::move(sf::Vector2i const &offset) noexcept
  {
    transformer_.move(sf::Vector2f(offset));
  }

  auto TileMap::isVisible() const noexcept -> bool
  {
    return isVisible_;
  }

  auto TileMap::getSizeInTiles() const noexcept -> sf::Vector2i
  {
    return mapSizeInTiles_;
  }

  auto TileMap::getPosition() const noexcept -> sf::Vector2i
  {
    return sf::Vector2i(transformer_.getPosition());
  }

  auto TileMap::getOrigin() const noexcept -> sf::Vector2i
  {
    return sf::Vector2i(transformer_.getOrigin());
  }

  auto TileMap::getSize() const noexcept -> sf::Vector2i
  {

    return sf::Vector2i(vertices_.getBounds().size);
  }

  auto TileMap::getTileSize() const noexcept -> sf::Vector2i
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

  auto TileMap::screenToTile(sf::Vector2i const &screenCoords) const noexcept
      -> sf::Vector2i
  {
    sf::Vector2i mapTopLeft = sf::Vector2i(transformer_.getTransform().transformPoint(
        vertices_[0].position));

    sf::Vector2i tileSize = getTileSize();
    sf::Vector2i coordsRelativeToMap = screenCoords - mapTopLeft;
    if (coordsRelativeToMap.x < 0)
    {
      coordsRelativeToMap.x -= tileSize.x;
    }

    if (coordsRelativeToMap.y < 0)
    {
      coordsRelativeToMap.y -= tileSize.y;
    }

    return coordsRelativeToMap.componentWiseDiv(tileSize);
  }

  auto TileMap::tileToScreenRect(sf::Vector2i const &tileCoords) const noexcept
      -> sf::IntRect
  {
    sf::Vector2i mapTopLeft = sf::Vector2i(transformer_.getTransform().transformPoint(
        vertices_[0].position));
    sf::Vector2i tileSize = getTileSize();
    sf::Vector2i tileTopLeft = mapTopLeft + tileSize.componentWiseMul(tileCoords);
    return sf::IntRect(tileTopLeft, tileSize);
  }

  void TileMap::fitItemToTile(
      BoardItem &item,
      sf::Vector2i const &tile,
      sf::Vector2f const &tileArea) const noexcept
  {
    sf::IntRect tileRect = tileToScreenRect(tile);
    sf::Vector2f tileSize = sf::Vector2f(tileRect.size).componentWiseMul(tileArea);

    SPDLOG_DEBUG("item.getSize() = ({}, {})", item.getSize().x, item.getSize().y);

    sf::Vector2f scaleFactors = tileSize.componentWiseDiv(sf::Vector2f(item.getSize()));
    item.scale(scaleFactors);
    item.setPosition(tileRect.position);
    item.setVisible(true);

    //==========
    SPDLOG_DEBUG(
        "Fit item '{}' to tile ({}, {}) at position ({}, {}) with size ({}, {})",
        item.getName(),
        tile.x, tile.y,
        item.getPosition().x, item.getPosition().y,
        item.getSize().x, item.getSize().y);
    //==========
  }
}
