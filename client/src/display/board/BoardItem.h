// BoardItem.h
#pragma once

#include <string>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include "BaseTextureAtlas.h"

namespace bgg
{
  class BoardItem final : public sf::Drawable
  {
    std::shared_ptr<const BaseTextureAtlas> atlas_;
    sf::Sprite sprite_;
    std::string name_;
    bool isVisible_;

  public:
    BoardItem(
        std::shared_ptr<const BaseTextureAtlas> atlas,
        sf::IntRect textureCellRect,
        std::string name,
        bool isVisible = true) noexcept
        : atlas_(std::move(atlas)),
          sprite_(*(atlas_->getTexture()), std::move(textureCellRect)),
          name_(std::move(name)),
          isVisible_(isVisible)
    {
    }

    void setVisible(bool visible) noexcept
    {
      isVisible_ = visible;
    }

    void setScale(sf::Vector2f const &factors) noexcept
    {
      sprite_.setScale(factors);
    }

    void setPosition(sf::Vector2i const &position) noexcept
    {
      sprite_.setPosition(sf::Vector2f(position));
    }

    void setOrigin(sf::Vector2i const &origin) noexcept
    {
      sprite_.setOrigin(sf::Vector2f(origin));
    }

    void scale(sf::Vector2f const &factors) noexcept
    {
      sprite_.scale(factors);
    }

    void move(sf::Vector2i const &offset) noexcept
    {
      sprite_.move(sf::Vector2f(offset));
    }

    [[nodiscard]]
    auto isVisible() const noexcept -> bool
    {
      return isVisible_;
    }

    [[nodiscard]]
    auto getName() const noexcept -> std::string
    {
      return name_;
    }

    [[nodiscard]]
    auto getPosition() const noexcept -> sf::Vector2i
    {
      return sf::Vector2i(sprite_.getPosition());
    }

    [[nodiscard]]
    auto getSize() const noexcept -> sf::Vector2i
    {
      return sf::Vector2i(sprite_.getGlobalBounds().size);
    }

    [[nodiscard]]
    auto getOrigin() const noexcept -> sf::Vector2i
    {
      return sf::Vector2i(sprite_.getOrigin());
    }

  private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const noexcept override
    {
      // states.transform *= sprite_.getTransform(); => Note: don't uncomment this otherwise you will apply the sprite_.getTransform() twice
      target.draw(sprite_, states);
    }
  };
} // namespace bgg
