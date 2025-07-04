// SfTileMap.h
#pragma once

#include <filesystem>

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>

namespace bgg
{
  class SfTileMap final : public sf::Drawable //, public sf::Transformable
  {
    sf::VertexArray vertices_;
    sf::Texture tileSet_;
    sf::Vector2u tileSizeInPixels_;
    sf::Vector2u mapSizeInPixels_;

  public:
    SfTileMap(std::filesystem::path const &tileSetPath,
              sf::Vector2u const &tileSizeInPixels,
              unsigned const *const tileLevels,
              sf::Vector2u const &mapSizeInTiles);

    // [[nodiscard]] auto getTileCoords(int x, int y) const noexcept -> sf::Vector2i;
    void fitRectangle(
        sf::Vector2u const &mapRegionTopLeft, sf::Vector2u const &mapRegionBotRight);

    // [[nodiscard]] auto getTileSize() const noexcept -> sf::Vector2u { return tileSet_.getSize(); }

  private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const noexcept override;
  };
}