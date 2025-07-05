// SfTileMap.h
#pragma once

#include <vector>
#include <memory>

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>

namespace bgg
{
  class SfTextureAtlas;
  class SfTileMap final : public sf::Drawable
  {
    std::shared_ptr<const SfTextureAtlas> const mapTextureAtlas_;
    sf::VertexArray vertices_;
    sf::Vector2i const mapSizeInTiles_;
    sf::Transformable transformer_;
    bool isVisible_;

  public:
    SfTileMap(std::shared_ptr<const SfTextureAtlas> mapTextureAtlas,
              sf::Vector2i mapSizeInTiles,
              std::vector<int> const &tileLayout);

    void setVisible(bool visible) noexcept;
    void setScale(sf::Vector2f const &factors) noexcept;
    void setPosition(sf::Vector2i const &position) noexcept;

    void scale(sf::Vector2f const &factors) noexcept;
    void move(sf::Vector2i const &offset) noexcept;

    [[nodiscard]] auto isVisible() const noexcept -> bool;
    [[nodiscard]] auto getSizeInTiles() const noexcept -> sf::Vector2i;
    [[nodiscard]] auto getPosition() const noexcept -> sf::Vector2i;
    [[nodiscard]] auto getOrigin() const noexcept -> sf::Vector2i;
    [[nodiscard]] auto getSize() const noexcept -> sf::Vector2i;
    [[nodiscard]] auto getTileSize() const noexcept -> sf::Vector2i;

    [[nodiscard]] auto screenToTile(sf::Vector2i const &screenCoords) const noexcept -> sf::Vector2i;
    [[nodiscard]] auto tileToScreenRect(sf::Vector2i const &tileCoords) const noexcept -> sf::IntRect;

  private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
  };
}