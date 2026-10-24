#include "mods/world/scenery/Scenery.hpp"

#include <algorithm>
#include <cmath>

#include "core/math/Scalar.hpp"

namespace wheel_of_fear {
  Scenery::Scenery(
    const SceneryPlacementDto &placement
  )
      : position_(placement.position)
      , mirrored_(placement.mirrored)
      , room_index_(placement.room_index)
      , color_(placement.color)
      , flicker_seed_(placement.flicker_seed) {
  }

  void Scenery::update(
    float delta_seconds
  ) {
    wobble_seconds_ = std::max(0.0F, wobble_seconds_ - delta_seconds);
  }

  void Scenery::bump(
    RandomGenerator &random
  ) {
    wobble_seconds_ = wobble_duration_seconds;
    reactToBump(random);
  }

  void Scenery::draw(
    Renderer               &renderer,
    const SpriteSheetProps &sprites,
    const SceneryViewDto   &view
  ) const {
    const float wobble =
      std::sin(wobble_seconds_ * 40) * wobble_seconds_ * 0.6F;
    renderer.drawShadowDisc(
      position_.add(Vec2::of(1, 3)),
      9,
      Color::black().withAlpha(0.45F),
      1.3F
    );
    renderer.drawSprite(
      sprite(sprites),
      position_,
      SpriteDrawDto {.rotation = wobble + tilt_, .mirrored = mirrored_}
    );
    const AtlasRegionProps* glow = glowSprite(sprites);
    if(glow == nullptr) return;
    const float blink =
      0.55F +
      0.45F *
      Scalar::hashTime(view.real_seconds + flicker_seed_ * 9, 6);
    const float strength = is_broken_ ? 0.04F : 0.18F;
    renderer.drawGlowSprite(
      *glow,
      position_,
      color_.withAlpha(blink * strength),
      SpriteDrawDto {.rotation = wobble + tilt_, .mirrored = mirrored_}
    );
  }
} // namespace wheel_of_fear
