// SfTileMap.h
#pragma once

#include <vector>

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>

namespace bgg
{
  class SfTextureAtlas;
  class SfTileMap final : public sf::Drawable, public sf::Transformable
  {
    sf::VertexArray vertices_;
    SfTextureAtlas const *const atlas_;
    sf::Vector2i const mapSizeInTiles_;

  public:
    SfTileMap(SfTextureAtlas const *atlas,
              sf::Vector2i mapSizeInTiles,
              std::vector<sf::Vector2i> const &tileLayout);

    [[nodiscard]] auto getMapSizeInTiles() const noexcept -> sf::Vector2i const &;

  private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
  };
}