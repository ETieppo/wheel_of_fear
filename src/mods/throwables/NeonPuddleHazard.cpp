#include "mods/throwables/NeonPuddleHazard.hpp"

#include <algorithm>
#include <cmath>

#include "core/math/ScalarMath.hpp"
#include "core/theme/Palette.hpp"

namespace wheel_of_fear {
  void NeonPuddleHazard::update(
    float delta_seconds
  ) {
    remaining_seconds_ -= delta_seconds;
    flash_seconds_ = std::max(0.0F, flash_seconds_ - delta_seconds);
  }

  void NeonPuddleHazard::emitLights(
    LightingSystem &lighting,
    float           real_seconds
  ) const {
    const float fade = ScalarMath::clamp(remaining_seconds_ / 8, 0, 1);

    const float shimmer =
      0.85F + (0.15F * std::sin((real_seconds * 4) + position_.x));

    LightProps glow;

    glow.position  = position_;
    glow.radius    = light_radius;
    glow.intensity = 1.1F * fade * shimmer;
    glow.color     = Palette::neon_magenta;
    glow.flicker   = 0.05F;
    lighting.add(glow);
    if(flash_seconds_ <= 0) return;

    LightProps flash;

    flash.position  = position_;
    flash.radius    = 170;
    flash.intensity = 3 * flash_seconds_ / flash_duration_seconds;
    flash.color     = Palette::neon_magenta.mix(ColorProps::white(), 0.5F);
    flash.freezes_creatures = true;
    lighting.add(flash);
  }
} // namespace wheel_of_fear
