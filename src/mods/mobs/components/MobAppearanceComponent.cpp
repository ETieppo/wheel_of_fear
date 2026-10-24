#include "mods/mobs/components/MobAppearanceComponent.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/render/Renderer.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/mobs/MobEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include <algorithm>

namespace wheel_of_fear {
  void
  MobAppearanceComponent::refreshVisibility(const GameContextDto &context) {
    const MobConditionComponent &condition = owner_.condition();

    if(
      condition.isBurning()
      || condition.isCrumbling()
      || condition.isBeingHugged()
      || owner_.isDisguised()
    ) {
      visibility_ = 1;
      return;
    }

    const float light = context.lighting.visibilityAt(
      context.world.map(),
      owner_.position(),
      owner_.traits().visible_under
    );

    visibility_ = ScalarMath::clamp(light * visibility_gain, 0, 1);
  }

  void MobAppearanceComponent::drawBody(
    GameContextDto         &context,
    const AtlasRegionProps &body,
    const AtlasRegionProps &glow,
    const MobLookDto       &look
  ) const {
    const float presence_amount = presence();
    if(presence_amount <= 0.01F) return;
    const MobConditionComponent &condition = owner_.condition();

    Renderer        &renderer      = context.renderer;
    const Vec2Props  draw_position = owner_.position();
    const float      rotation      = owner_.heading() + look.rotation_offset;
    const ColorProps tint
      = condition.isBurning() ? ColorProps::hex(0xfff2d8) : look.tint;

    renderer.drawShadowDisc(
      draw_position.add(Vec2Props::of(1.5F, 3)),
      owner_.traits().collision_radius * 1.25F * look.scale,
      ColorProps::black().withAlpha(0.5F * presence_amount),
      1.3F
    );

    renderer.drawSprite(
      body,
      draw_position,
      SpriteDrawDto {
        .rotation = rotation,
        .color    = tint.withAlpha(presence_amount),
        .petrify  = condition.petrifyAmount(),
        .glow     = condition.isBurning() ? 0.8F : 0.0F,
        .scale    = look.scale,
      }
    );

    if(condition.petrifyAmount() > 0.5F) return;

    renderer.drawGlowSprite(
      glow,
      draw_position,
      look.glow_color.withAlpha(
        glowLevel() * look.glow_strength * std::max(presence_amount, 0.35F)
      ),
      SpriteDrawDto {.rotation = rotation, .scale = look.scale}
    );
  }

  float MobAppearanceComponent::glowLevel() const {
    const MobConditionComponent &condition = owner_.condition();

    if(condition.isFrozen()) return 0.35F;
    if(
      condition.frenzySeconds()
      > 0
      || owner_.brain().intent()
      == MobIntentEnum::chasing
    )
      return 1.0F;

    return 0.75F;
  }
} // namespace wheel_of_fear
