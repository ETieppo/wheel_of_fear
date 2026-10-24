#include "mods/mobs/species/BloaterMob.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/ArtLibrary.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/world/WorldSystem.hpp"
#include <algorithm>
#include <cmath>

namespace wheel_of_fear {
  void BloaterMob::lightFuse(float minimum_fuse_seconds) {
    fuse_seconds_ = std::max(fuse_seconds_, minimum_fuse_seconds);
  }

  void BloaterMob::ignite([[maybe_unused]] GameContextDto &context) {
    lightFuse(chained_fuse_seconds);
  }

  void BloaterMob::catchBlast([[maybe_unused]] GameContextDto &context) {
    lightFuse(chained_fuse_seconds);
  }

  void BloaterMob::think(GameContextDto &context) {
    if(isBusy()) return;

    const float delta       = context.clock.deltaSeconds();
    const float ultraviolet = context.lighting.visibilityAt(
      context.world.map(),
      position(),
      LightFilterEnum::ultraviolet_only
    );

    if(ultraviolet > ultraviolet_ignition_threshold) fuse_seconds_ += delta;
    else if(fuse_seconds_ < fuse_limit_seconds * 0.9F)
      fuse_seconds_ = std::max(0.0F, fuse_seconds_ - (delta * 0.5F));

    if(isLit() && voice().vocalCooldown() <= 0) {
      voice().setVocalCooldown(0.16F);
      voice().playSound(
        context,
        SoundEnum::switch_click,
        0.6F,
        0.6F + fuse_seconds_
      );
    }

    if(fuse_seconds_ >= fuse_limit_seconds) {
      context.mobs.explosions().detonate(context, *this);
      return;
    }

    brain().hunt(context);
  }

  void BloaterMob::onContact(GameContextDto &context) {
    context.mobs.explosions().detonate(context, *this);
  }

  void BloaterMob::draw(GameContextDto &context) const {
    const SpriteSheetProps &sprites = context.art.sprites();
    const std::size_t       frame
      = animationFrame(6 + (fuse_seconds_ * 20), sprites.bloater_drift.size());
    const float swell = 1
      + (0.05F * std::sin(animationSeconds() * (2.5F + (fuse_seconds_ * 14))));
    const ColorProps glow_color
      = isLit() ? ColorProps::hex(0xff5a2a) : ColorProps::hex(0xffb020);

    appearance().drawBody(
      context,
      sprites.bloater_drift[frame],
      sprites.bloater_drift_glow[frame],
      MobLookDto {
        .glow_color    = glow_color,
        .scale         = swell,
        .glow_strength = 1 + fuse_seconds_,
      }
    );
  }
} // namespace wheel_of_fear
