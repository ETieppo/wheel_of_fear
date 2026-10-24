#include "mods/mobs/species/Bloater.hpp"

#include <algorithm>
#include <cmath>

#include "core/time/GameClock.hpp"
#include "mods/art/Art.hpp"
#include "mods/lighting/Lighting.hpp"
#include "mods/mobs/Mobs.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {
  void Bloater::lightFuse(
    float minimum_fuse_seconds
  ) {
    fuse_seconds_ = std::max(fuse_seconds_, minimum_fuse_seconds);
  }

  void Bloater::ignite(
    GameContextDto &
  ) {
    lightFuse(chained_fuse_seconds);
  }

  void Bloater::catchBlast(
    GameContextDto &
  ) {
    lightFuse(chained_fuse_seconds);
  }

  void Bloater::think(
    GameContextDto &context
  ) {
    if(isBusy()) return;
    const float delta       = context.clock.deltaSeconds();
    const float ultraviolet = context.lighting.visibilityAt(
      context.world.map(),
      position(),
      LightFilterEnum::ultraviolet_only
    );
    if(ultraviolet > ultraviolet_ignition_threshold) fuse_seconds_ += delta;
    else if(fuse_seconds_ < fuse_limit_seconds * 0.9F)
      fuse_seconds_ = std::max(0.0F, fuse_seconds_ - delta * 0.5F);
    if(isLit() && vocalCooldown() <= 0) {
      setVocalCooldown(0.16F);
      playSound(context, SoundEnum::switch_click, 0.6F, 0.6F + fuse_seconds_);
    }
    if(fuse_seconds_ >= fuse_limit_seconds) {
      context.mobs.detonate(context, *this);
      return;
    }
    thinkAsHunter(context);
  }

  void Bloater::onContact(
    GameContextDto &context
  ) {
    context.mobs.detonate(context, *this);
  }

  void Bloater::draw(
    GameContextDto &context
  ) const {
    const SpriteSheetProps &sprites = context.art.sprites();
    const std::size_t       frame =
      animationFrame(6 + fuse_seconds_ * 20, sprites.bloater_drift.size());
    const float swell =
      1 + 0.05F * std::sin(animationSeconds() * (2.5F + fuse_seconds_ * 14));
    const Color glow_color =
      isLit() ? Color::hex(0xff5a2a) : Color::hex(0xffb020);
    drawBody(
      context,
      sprites.bloater_drift[frame],
      sprites.bloater_drift_glow[frame],
      MobLookDto {
        .glow_color    = glow_color,
        .scale         = swell,
        .glow_strength = 1 + fuse_seconds_
      }
    );
  }
} // namespace wheel_of_fear
