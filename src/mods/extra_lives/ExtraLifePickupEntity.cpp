#include "mods/extra_lives/ExtraLifePickupEntity.hpp"
#include "core/theme/Palette.hpp"
#include "mods/lighting/LightProps.hpp"
#include <cmath>

namespace wheel_of_fear {
  float ExtraLifePickupEntity::pulse(float real_seconds) {
    return 0.6F + (0.4F * std::sin(real_seconds * 4));
  }

  void ExtraLifePickupEntity::emitLight(
    LightingSystem &lighting,
    float           real_seconds
  ) const {
    LightProps light;

    light.position      = position_;
    light.radius        = 40;
    light.intensity     = 0.8F * pulse(real_seconds);
    light.color         = Palette::accent;
    light.casts_shadows = false;
    lighting.add(light);
  }

  void ExtraLifePickupEntity::draw(
    Renderer               &renderer,
    const SpriteSheetProps &sprites,
    float                   real_seconds
  ) const {
    const float glow = pulse(real_seconds);

    renderer.drawShadowDisc(
      position_.add(Vec2Props::of(0.5F, 1.5F)),
      5,
      ColorProps::black().withAlpha(0.5F),
      1.2F
    );

    renderer.drawGlowDisc(
      position_,
      9,
      Palette::accent.withAlpha(0.45F * glow),
      1.5F
    );

    renderer.drawGlowSprite(
      sprites.flare,
      position_,
      Palette::accent.withAlpha(glow),
      SpriteDrawDto {.rotation = real_seconds * 0.8F, .scale = 0.6F}
    );
  }
} // namespace wheel_of_fear
