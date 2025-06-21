// SfTileMap.h
#pragma once

#include <filesystem>

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>

namespace iab
{
  class SfTileMap : public sf::Drawable, public sf::Transformable
  {
    sf::VertexArray vertices_;
    sf::Texture tileSet_;
    sf::Vector2i startPoint_;
    sf::Vector2u tileSize_;

  public:
    SfTileMap(std::filesystem::path const &tileSetPath,
              sf::Vector2i const &startPoint,
              sf::Vector2u const &tileSize,
              int const *const tileLevels,
              int const tilesPerRow,
              int const tilesPerCol);

    [[nodiscard]] auto getTileCoords(int x, int y) const noexcept -> sf::Vector2i;

    // [[nodiscard]] auto getTileSize() const noexcept -> sf::Vector2u { return tileSet_.getSize(); }

  private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const noexcept override;
  };
}