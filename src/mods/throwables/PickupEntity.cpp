#include "mods/throwables/PickupEntity.hpp"
#include <cmath>

namespace wheel_of_fear {
  void PickupEntity::draw(
    Renderer               &renderer,
    const SpriteSheetProps &sprites,
    ColorProps              color,
    std::size_t             index,
    float                   real_seconds
  ) const {
    const auto              order  = static_cast<float>(index);
    const AtlasRegionProps &sprite = item_ == ItemEnum::neon_grenade
      ? sprites.neon_grenade
      : sprites.lure_beacon;

    renderer.drawShadowDisc(
      position_.add(Vec2Props::of(0.5F, 1.5F)),
      4,
      ColorProps::black().withAlpha(0.5F),
      1.2F
    );

    renderer
      .drawSprite(sprite, position_, SpriteDrawDto {.rotation = order * 1.7F});
    const float glint = 0.5F + (0.5F * std::sin((real_seconds * 3) + order));

    renderer.drawGlowDisc(position_, 6, color.withAlpha(0.35F * glint), 1.5F);
  }
} // namespace wheel_of_fear
