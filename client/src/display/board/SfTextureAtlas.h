// SfTextureAtlas.h
#pragma once

#include <SFML/Graphics/Texture.hpp>

namespace bgg
{
  class SfTextureAtlas
  {
    sf::Texture texture_;

  public:
    SfTextureAtlas(sf::Texture texture, bool smooth = true) noexcept
        : texture_(std::move(texture))
    {
      texture_.setSmooth(smooth);
    }

    ~SfTextureAtlas() = default;

    [[nodiscard]]
    auto getTexture() const noexcept -> sf::Texture const *
    {
      return &texture_;
    }

    virtual auto getRegion(int regionIndex) const -> sf::IntRect = 0;
  };

} // namespace bgg
