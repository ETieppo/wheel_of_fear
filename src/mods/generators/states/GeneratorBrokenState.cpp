#include "mods/generators/states/GeneratorBrokenState.hpp"
#include "core/input/InputController.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/effects/EffectSystem.hpp"
#include "mods/generators/GeneratorEntity.hpp"
#include "mods/hud/HudSystem.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/noise/NoiseSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include <algorithm>
#include <format>

namespace wheel_of_fear {
  ColorProps GeneratorBrokenState::statusColor(
    [[maybe_unused]] const GeneratorEntity &generator
  ) const {
    return Palette::spark_blue;
  }

  void GeneratorBrokenState::enter(
    GeneratorEntity &generator,
    GameContextDto  &context
  ) {
    generator.setProgress(std::max(0.0F, generator.progress() - 0.12F));
    wiring_progress_ = 0;
    context.world.shake(0.4F);
    context.effects
      .emitSparks(context.world, generator.position(), 24, Palette::spark_blue);
    generator.playSound(context, SoundEnum::generator_breakdown, 1.0F, 1);

    context.noise.emit(
      generator.position(),
      GeneratorEntity::failure_noise_radius,
      NoiseSourceEnum::repair_failure
    );

    context.hud.announce(
      Palette::danger,
      std::format(
        "GENERATOR {} BROKE DOWN  -  HOLD E TO REWIRE",
        generator.label()
      )
    );
  }

  void GeneratorBrokenState::update(
    GeneratorEntity          &generator,
    GameContextDto           &context,
    const GeneratorUpdateDto &update
  ) {
    const float              delta     = context.clock.deltaSeconds();
    GeneratorMachineryProps &machinery = generator.machinery();
    machinery.rotor_speed
      = ScalarMath::approach(machinery.rotor_speed, 0, delta * 8);

    emitFaultSparks(generator, context, delta);
    emitFaultLight(generator, context);

    if(!update.is_player_near) return;
    if(!generator.isBeingRepaired()) {
      context.hud.showPrompt(
        InputActionEnum::interact,
        std::format("hold to rewire generator {}", generator.label())
      );

      return;
    }

    rewire(generator, context, delta);
  }

  void GeneratorBrokenState::emitFaultSparks(
    GeneratorEntity &generator,
    GameContextDto  &context,
    float            delta_seconds
  ) {
    GeneratorMachineryProps &machinery = generator.machinery();
    RandomGenerator         &random    = context.world.random();
    machinery.spark_timer -= delta_seconds;

    if(machinery.spark_timer > 0) return;
    machinery.spark_timer = random.range(0.08F, 0.5F);

    const float offset_x = random.range(-10, 10);
    const float offset_y = random.range(-10, 6);

    context.effects.emitSparks(
      context.world,
      generator.position().add(Vec2Props::of(offset_x, offset_y)),
      4,
      Palette::spark_blue
    );

    if(random.chance(0.3F))
      generator.playSound(
        context,
        SoundEnum::electric_buzz,
        0.4F,
        random.range(0.8F, 1.3F)
      );
  }

  void GeneratorBrokenState::emitFaultLight(
    const GeneratorEntity &generator,
    GameContextDto        &context
  ) {
    LightProps light;

    light.position  = generator.position();
    light.radius    = 50;
    light.intensity = 0.6F;
    light.color     = Palette::spark_blue;
    light.flicker   = 0.9F;
    context.lighting.add(light);
  }

  void GeneratorBrokenState::rewire(
    GeneratorEntity &generator,
    GameContextDto  &context,
    float            delta_seconds
  ) {
    context.player.beginRepair(generator.position());
    wiring_progress_ += delta_seconds;
    repair_sound_timer_ -= delta_seconds;

    if(repair_sound_timer_ <= 0) {
      repair_sound_timer_ = 0.5F;

      generator.playSound(
        context,
        SoundEnum::generator_tick,
        0.7F,
        0.6F + wiring_progress_
      );

      context.noise.emit(
        generator.position(),
        GeneratorEntity::repair_noise_radius,
        NoiseSourceEnum::repairing
      );
    }

    context.hud.showPrompt(
      std::nullopt,
      std::format(
        "REWIRING GENERATOR {}  {:.0f}%",
        generator.label(),
        wiring_progress_ / wiring_repair_seconds * 100
      )
    );

    if(wiring_progress_ < wiring_repair_seconds) return;
    wiring_progress_ = 0;
    generator.enterCharging();

    context.hud.announce(
      Palette::success,
      std::format("GENERATOR {} REWIRED", generator.label())
    );

    generator.playSound(context, SoundEnum::part_pickup, 0.5F, 0.6F);
  }
} // namespace wheel_of_fear
