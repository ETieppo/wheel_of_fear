#include "mods/world/Locker.hpp"

#include <algorithm>

#include "core/math/Scalar.hpp"

namespace wheel_of_fear {
  Vec2 Locker::entryPoint() const {
    return position_.add(Vec2::of(0, faces_down_ ? 13.0F : -13.0F));
  }

  void Locker::update(float delta_seconds) {
    door_open_amount_ = std::max(0.0F, door_open_amount_ - delta_seconds * 2);
  }

  void Locker::draw(
    Renderer               &renderer,
    const SpriteSheetProps &sprites
  ) const {
    const AtlasRegionProps &region =
      door_open_amount_ > 0.3F ? sprites.locker_open : sprites.locker_closed;
    renderer.drawShadowDisc(
      position_.add(Vec2::of(0, 2)), 9, Color::black().withAlpha(0.5F), 1.3F
    );
    renderer.drawSprite(
      region,
      position_,
      SpriteDrawDto {.rotation = faces_down_ ? 0.0F : Scalar::pi}
    );
  }
} // namespace wheel_of_fear
