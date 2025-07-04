// SfBoardItem.h
#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace bgg
{
  class SfBoardItem final : public sf::Drawable
  {
    sf::Sprite sprite_;
    bool isVisible_;

  public:
    SfBoardItem(
        const sf::Texture &texture,
        const sf::IntRect &rectangle,
        bool isVisible = true) noexcept
        : sprite_(texture, rectangle),
          isVisible_(isVisible)
    {
    }
    
    auto isVisible() const noexcept -> bool { return isVisible_; }
    void setVisible(bool visible) noexcept { isVisible_ = visible; }
    void setPosition(sf::Vector2f const &position) noexcept { sprite_.setPosition(position); }
    auto getPosition() const noexcept -> sf::Vector2f { return sprite_.getPosition(); }

  private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const noexcept override
    {
      states.transform *= sprite_.getTransform();
      target.draw(sprite_, states);
    }
  };
} // namespace bgg
