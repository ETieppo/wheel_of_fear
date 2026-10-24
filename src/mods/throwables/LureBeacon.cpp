#include "mods/throwables/LureBeacon.hpp"

#include "core/audio/SoundPlayer.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/effects/Effects.hpp"
#include "mods/noise/Noise.hpp"
#include "mods/player/Player.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {
  void LureBeacon::update(
    GameContextDto &context
  ) {
    const float delta = context.clock.deltaSeconds();
    remaining_seconds_ -= delta;
    pulse_timer_ -= delta;
    pulse_age_seconds_ += delta;
    if(pulse_timer_ <= 0 && remaining_seconds_ > 0) {
      pulse_timer_       = pulse_seconds;
      pulse_age_seconds_ = 0;
      context.noise.emit(position_, noise_radius, NoiseSourceEnum::lure_beacon);
      context.sound_player.playAt(
        SoundEnum::lure_beep,
        0.7F,
        context.player.position(),
        position_,
        1
      );
    }
    if(isExpired())
      context.effects
        .emitSparks(context.world, position_, 4, Palette::neon_lime);
  }

  void LureBeacon::emitLight(
    Lighting &lighting
  ) const {
    LightProps light;
    light.position      = position_;
    light.radius        = 34;
    light.intensity     = 0.7F * blink();
    light.color         = Palette::neon_lime;
    light.casts_shadows = false;
    lighting.add(light);
  }

  void LureBeacon::draw(
    Renderer               &renderer,
    const SpriteSheetProps &sprites
  ) const {
    renderer.drawSprite(sprites.lure_beacon, position_);
    const float glow = pulse_age_seconds_ < 0.15F ? 1.0F : 0.25F;
    renderer
      .drawGlowDisc(position_, 4, Palette::neon_lime.withAlpha(glow), 1.3F);
    const float ring = pulse_age_seconds_ / pulse_seconds;
    renderer.drawGlowSprite(
      sprites.soft_circle,
      position_,
      Palette::neon_lime.withAlpha(0.25F * (1 - ring)),
      SpriteDrawDto {.scale = 0.2F + ring * 1.2F}
    );
  }
} // namespace wheel_of_fear
