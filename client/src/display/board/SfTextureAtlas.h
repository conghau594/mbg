// SfTextureAtlas.h
#pragma once

#include <SFML/Graphics/Texture.hpp>

namespace bgg
{
  class SfTextureAtlas
  {
    sf::Texture texture_;

  public:
    virtual ~SfTextureAtlas() = default;

    SfTextureAtlas(sf::Texture texture, bool smooth = false) noexcept
        : texture_(std::move(texture))
    {
      texture_.setSmooth(smooth);
    }

    [[nodiscard]]
    auto getTexture() const noexcept -> sf::Texture const *
    {
      return &texture_;
    }

    [[nodiscard]]
    virtual auto getRegion(int regionIndex) const -> sf::IntRect = 0;
  };

} // namespace bgg
