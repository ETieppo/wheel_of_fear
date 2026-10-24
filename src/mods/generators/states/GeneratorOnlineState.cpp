#include "mods/generators/states/GeneratorOnlineState.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/generators/GeneratorEntity.hpp"
#include "mods/lighting/LightingSystem.hpp"

namespace wheel_of_fear {
  ColorProps GeneratorOnlineState::statusColor(
    [[maybe_unused]] const GeneratorEntity &generator
  ) const {
    return Palette::neon_cyan;
  }

  void GeneratorOnlineState::update(
    GeneratorEntity                           &generator,
    GameContextDto                            &context,
    [[maybe_unused]] const GeneratorUpdateDto &update
  ) {
    const float delta = context.clock.deltaSeconds();
    seconds_online_ += delta;

    GeneratorMachineryProps &machinery = generator.machinery();
    machinery.rotor_speed
      = ScalarMath::approach(machinery.rotor_speed, 9, delta * 2);

    const float warm_up = ScalarMath::clamp(seconds_online_ / 1.5F, 0, 1);
    LightProps  light;

    light.position      = generator.position();
    light.radius        = 70 * warm_up;
    light.intensity     = 0.9F;
    light.color         = ColorProps::hex(0x7af0ff);
    light.flicker       = 0.04F;
    light.casts_shadows = false;
    context.lighting.add(light);
  }
} // namespace wheel_of_fear
