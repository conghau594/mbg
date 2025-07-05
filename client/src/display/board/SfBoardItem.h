// SfBoardItem.h
#pragma once

#include <string>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace bgg
{
  class SfBoardItem final : public sf::Drawable
  {
    sf::Sprite sprite_;
    std::string name_;
    bool isVisible_;

  public:
    SfBoardItem(
        sf::Texture const &texture,
        sf::IntRect const &rectangle,
        std::string name,
        bool isVisible = true) noexcept
        : sprite_(texture, rectangle),
          name_(std::move(name)),
          isVisible_(isVisible)
    {
    }

    void setVisible(bool visible) noexcept { isVisible_ = visible; }
    void setScale(sf::Vector2f const &factors) noexcept { sprite_.setScale(factors); }
    void setPosition(sf::Vector2f const &position) noexcept { sprite_.setPosition(position); }

    auto isVisible() const noexcept -> bool { return isVisible_; }
    auto getName() const noexcept -> std::string { return name_; }
    auto getPosition() const noexcept -> sf::Vector2f { return sprite_.getPosition(); }
    auto getSize() const noexcept -> sf::Vector2f { return sprite_.getGlobalBounds().size; }
    auto getOrigin() const noexcept -> sf::Vector2f { return sprite_.getOrigin(); }

  private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const noexcept override
    {
      // states.transform *= sprite_.getTransform(); => Note: don't uncomment this otherwise you will apply the sprite_.getTransform() twice
      target.draw(sprite_, states);
    }
  };
} // namespace bgg
