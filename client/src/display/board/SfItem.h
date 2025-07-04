// SfItem.h
#pragma once

#include <SFML/Graphics/Sprite.hpp>

namespace bgg
{
  class SfItem final : public sf::Drawable
  {
    sf::Sprite sprite_;
    size_t id_;
    bool isVisible_;

  public:
    // SfItem()
    auto getId() const noexcept -> size_t { return id; }
    auto isVisible() const noexcept -> bool { return isVisible_; }
    void setVisible(bool visible) noexcept { isVisible_ = visible; }
    void setPosition(Vector2f const &position) noexcept { sprite_.setPosition(position); }
    auto getPosition() const noexcept -> sf::Vector2f { return sprite_.getPosition(); }

  private:
    void draw(RenderTarget &target, RenderStates states) const override
    {
      states.transform *= sprite_.getTransform();
      target.draw(sprite_, states);
    }
  };
} // namespace bgg
