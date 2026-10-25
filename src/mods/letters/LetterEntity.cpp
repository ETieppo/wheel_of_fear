#include "mods/letters/LetterEntity.hpp"
#include "core/theme/Palette.hpp"
#include "mods/lighting/LightProps.hpp"
#include <cmath>

namespace wheel_of_fear {
  Vec2Props LetterEntity::spotlightOrigin() const {
    return position_.add(Vec2Props::of(0, -spotlight_height));
  }

  void LetterEntity::emitSpotlight(
    LightingSystem &lighting,
    float           real_seconds
  ) const {
    LightProps spotlight;

    spotlight.position          = spotlightOrigin();
    spotlight.radius            = spotlight_reach;
    spotlight.intensity         = spotlight_intensity;
    spotlight.color             = Palette::warm_light;
    spotlight.direction         = position_.sub(spotlightOrigin()).normalized();
    spotlight.cone_outer_cosine = spotlight_outer_cosine;
    spotlight.cone_inner_cosine = spotlight_inner_cosine;
    spotlight.casts_shadows     = false;
    spotlight.flicker = 0.04F + (0.02F * std::sin(real_seconds * 1.3F));
    lighting.add(spotlight);
  }

  void LetterEntity::draw(
    Renderer               &renderer,
    const SpriteSheetProps &sprites,
    float                   real_seconds
  ) const {
    const float shimmer = 0.85F + (0.15F * std::sin(real_seconds * 2));

    renderer.drawShadowDisc(
      position_.add(Vec2Props::of(1, 2)),
      7,
      ColorProps::black().withAlpha(0.45F),
      1.2F
    );

    renderer.drawSprite(
      sprites.letter,
      position_,
      SpriteDrawDto {.rotation = letter_rotation}
    );

    renderer.drawGlowDisc(
      position_,
      14,
      Palette::warm_light.withAlpha(0.18F * shimmer),
      1.6F
    );
  }
} // namespace wheel_of_fear
