#include "mods/throwables/Pickup.hpp"

#include <cmath>

#include "core/items/ItemCatalog.hpp"

namespace wheel_of_fear {
  void Pickup::draw(
    Renderer               &renderer,
    const SpriteSheetProps &sprites,
    std::size_t             index,
    float                   real_seconds
  ) const {
    const auto              order  = static_cast<float>(index);
    const AtlasRegionProps &sprite = item_ == ItemEnum::neon_grenade
                                     ? sprites.neon_grenade
                                     : sprites.lure_beacon;
    renderer.drawShadowDisc(
      position_.add(Vec2::of(0.5F, 1.5F)),
      4,
      Color::black().withAlpha(0.5F),
      1.2F
    );
    renderer
      .drawSprite(sprite, position_, SpriteDrawDto {.rotation = order * 1.7F});
    const float glint = 0.5F + 0.5F * std::sin(real_seconds * 3 + order);
    renderer.drawGlowDisc(
      position_,
      6,
      ItemCatalog::of(item_).color().withAlpha(0.35F * glint),
      1.5F
    );
  }
} // namespace wheel_of_fear
