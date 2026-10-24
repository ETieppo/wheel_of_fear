#include "mods/throwables/projectiles/Projectile.hpp"

#include <algorithm>

#include "core/math/ScalarMath.hpp"

namespace wheel_of_fear {
  Projectile::Projectile(
    ItemEnum  item,
    Vec2Props start,
    Vec2Props target
  )
      : Entity(
          start,
          2
        )
      , item_(item)
      , start_(start)
      , target_(target)
      , total_seconds_(
          std::max(
            0.15F,
            start.distance(target) / flight_speed
          )
        ) {
  }

  void Projectile::advance(
    float delta_seconds
  ) {
    flight_seconds_ = std::min(total_seconds_, flight_seconds_ + delta_seconds);
    setHeading(heading() + (delta_seconds * 16));
    setPosition(groundPosition());
  }

  float Projectile::progress() const {
    return ScalarMath::clamp(flight_seconds_ / total_seconds_, 0, 1);
  }

  Vec2Props Projectile::groundPosition() const {
    return start_.lerp(target_, progress());
  }

  Vec2Props Projectile::airPosition() const {
    const float t = progress();
    return groundPosition().sub(Vec2Props::of(0, 4 * t * (1 - t) * arc_height));
  }

  void Projectile::draw(
    Renderer               &renderer,
    const SpriteSheetProps &sprites
  ) const {
    renderer.drawShadowDisc(
      groundPosition(),
      3,
      ColorProps::black().withAlpha(0.4F),
      1.2F
    );

    renderer.drawSprite(
      sprite(sprites),
      airPosition(),
      SpriteDrawDto {.rotation = heading()}
    );

    drawTrail(renderer);
  }
} // namespace wheel_of_fear
