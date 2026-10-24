#include "mods/world/scenery/NeonSignScenery.hpp"
#include "core/math/ScalarMath.hpp"

namespace wheel_of_fear {
  float NeonSignScenery::brightness(const SceneryViewDto &view) const {
    if(view.is_room_powered) return 1;

    const float sputter
      = ScalarMath::hashTime(view.real_seconds + (flickerSeed() * 17), 9);

    return sputter > 0.86F ? 0.05F : 0.35F;
  }

  void NeonSignScenery::draw(
    Renderer               &renderer,
    const SpriteSheetProps &sprites,
    const SceneryViewDto   &view
  ) const {
    const float level = isBroken() ? 0.0F : brightness(view);

    if(level <= 0.05F) {
      renderer.drawSprite(
        sprite(sprites),
        position(),
        SpriteDrawDto {.color = ColorProps::hex(0x2c3038)}
      );

      return;
    }

    renderer.drawSprite(
      sprite(sprites),
      position(),
      SpriteDrawDto {.color = color(), .glow = level}
    );

    renderer.drawGlowSprite(
      sprites.light[1],
      position(),
      color().withAlpha(0.18F * level),
      SpriteDrawDto {.scale = 1.3F}
    );
  }

  void NeonSignScenery::emitLight(
    LightingSystem       &lighting,
    const SceneryViewDto &view
  ) const {
    if(isBroken()) return;

    const float level = brightness(view);
    if(level <= 0.05F) return;
    LightProps light;

    light.position      = position().add(Vec2Props::of(0, 14));
    light.radius        = light_radius;
    light.intensity     = 0.55F * level;
    light.color         = color();
    light.flicker       = 0.15F;
    light.casts_shadows = false;
    lighting.add(light);
  }
} // namespace wheel_of_fear
