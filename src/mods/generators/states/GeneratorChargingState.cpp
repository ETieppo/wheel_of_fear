#include "mods/generators/states/GeneratorChargingState.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/input/InputController.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/effects/EffectSystem.hpp"
#include "mods/generators/GeneratorEntity.hpp"
#include "mods/hud/HudSystem.hpp"
#include "mods/noise/NoiseSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include <algorithm>
#include <format>

namespace wheel_of_fear {
  ColorProps
  GeneratorChargingState::statusColor(const GeneratorEntity &generator) const {
    return generator.state() == GeneratorStateEnum::offline
      ? Palette::neon_red
      : Palette::neon_amber;
  }

  void GeneratorChargingState::update(
    GeneratorEntity          &generator,
    GameContextDto           &context,
    const GeneratorUpdateDto &update
  ) {
    if(!generator.isBeingRepaired()) {
      idle(generator, context, update);
      return;
    }

    charge(generator, context, update);
  }

  void GeneratorChargingState::idle(
    GeneratorEntity          &generator,
    GameContextDto           &context,
    const GeneratorUpdateDto &update
  ) {
    const float         delta       = context.clock.deltaSeconds();
    SkillCheckMinigame &skill_check = generator.skillCheck();

    if(generator.wasRepairedLastFrame() && skill_check.isActive())
      resolveSkillCheck(generator, context, SkillCheckResultEnum::failed);

    skill_check.cancel();
    GeneratorMachineryProps &machinery = generator.machinery();
    machinery.rotor_speed
      = ScalarMath::approach(machinery.rotor_speed, 0, delta * 3);

    if(
      generator.state()
      == GeneratorStateEnum::charging
      && generator.progress()
      > 0
    )
      regressWhileUnattended(generator, context);

    if(update.is_player_near && context.player.canBeTargeted())
      context.hud.showPrompt(
        InputActionEnum::interact,
        std::format("hold to charge generator {}", generator.label())
      );
  }

  void GeneratorChargingState::charge(
    GeneratorEntity          &generator,
    GameContextDto           &context,
    const GeneratorUpdateDto &update
  ) {
    const float delta = context.clock.deltaSeconds();

    context.player.beginRepair(generator.position());
    generator.enterCharging();
    seconds_unattended_           = 0;
    has_warned_regression_        = false;
    const float previous_progress = generator.progress();

    generator.setProgress(
      std::min(1.0F, generator.progress() + (delta / update.seconds_to_charge))
    );

    GeneratorMachineryProps &machinery = generator.machinery();
    machinery.rotor_speed              = ScalarMath::approach(
      machinery.rotor_speed,
      2 + (generator.progress() * 7),
      delta * 6
    );

    playRatchet(generator, context, delta);
    updateSkillCheck(generator, context);
    if(generator.state() != GeneratorStateEnum::charging) return;

    context.hud.showPrompt(
      std::nullopt,
      std::format(
        "CHARGING GENERATOR {}  {:.0f}%",
        generator.label(),
        generator.progress() * 100
      )
    );

    generator.triggerCrossedIncidents(context, previous_progress);

    if(
      generator.state()
      == GeneratorStateEnum::charging
      && generator.progress()
      >= 1
    )
      generator.bringOnline(context);
  }

  void GeneratorChargingState::playRatchet(
    GeneratorEntity &generator,
    GameContextDto  &context,
    float            delta_seconds
  ) {
    ratchet_timer_ -= delta_seconds * (1 + (generator.progress() * 2.5F));
    if(ratchet_timer_ > 0) return;
    ratchet_timer_ = 0.34F;

    generator.playSound(
      context,
      SoundEnum::generator_tick,
      0.5F,
      0.85F + (generator.progress() * 0.5F)
    );

    context.noise.emit(
      generator.position(),
      GeneratorEntity::repair_noise_radius,
      NoiseSourceEnum::repairing
    );
  }

  void GeneratorChargingState::regressWhileUnattended(
    GeneratorEntity &generator,
    GameContextDto  &context
  ) {
    const float delta = context.clock.deltaSeconds();

    seconds_unattended_ += delta;
    if(seconds_unattended_ <= regression_delay_seconds) return;

    generator.setProgress(
      std::max(0.0F, generator.progress() - (delta * regression_per_second))
    );

    if(!has_warned_regression_) {
      has_warned_regression_ = true;

      context.hud.announce(
        Palette::spark_yellow,
        std::format("GENERATOR {} IS LOSING CHARGE", generator.label())
      );
    }

    RandomGenerator &random = context.world.random();

    if(random.chance(delta * 1.5F))
      context.effects.emitSparks(
        context.world,
        generator.position().add(Vec2Props::of(random.range(-10, 10), -6)),
        2,
        Palette::spark_yellow
      );
  }

  void GeneratorChargingState::updateSkillCheck(
    GeneratorEntity &generator,
    GameContextDto  &context
  ) {
    const float         delta       = context.clock.deltaSeconds();
    SkillCheckMinigame &skill_check = generator.skillCheck();

    if(!skill_check.isActive()) {
      if(!skill_check.tickCountdown(delta)) return;

      skill_check.begin(context.world.phase(), context.world.random());
      context.sound_player.play(SoundEnum::qte_alert, 0.7F, 0, 1);
      return;
    }

    const auto result = skill_check.advanceNeedle(
      delta,
      context.input.wasPressed(InputActionEnum::hold_breath)
    );

    if(result) resolveSkillCheck(generator, context, *result);
  }

  void GeneratorChargingState::resolveSkillCheck(
    GeneratorEntity     &generator,
    GameContextDto      &context,
    SkillCheckResultEnum result
  ) {
    generator.skillCheck().resolve(result, context.world.random());

    switch(result) {
      case SkillCheckResultEnum::perfect:
        generator.setProgress(
          std::min(0.999F, generator.progress() + perfect_bonus)
        );
        context.sound_player.play(SoundEnum::qte_success, 0.7F, 0, 1.25F);
        break;

      case SkillCheckResultEnum::good:
        context.sound_player.play(SoundEnum::qte_success, 0.55F, 0, 1);
        break;

      case SkillCheckResultEnum::failed:
        shortCircuit(generator, context);
        break;

      case SkillCheckResultEnum::none: break;
    }
  }

  void GeneratorChargingState::shortCircuit(
    GeneratorEntity &generator,
    GameContextDto  &context
  ) {
    generator.setProgress(
      std::max(0.0F, generator.progress() - failure_regression)
    );

    context.sound_player.play(SoundEnum::qte_fail, 0.9F, 0, 1);
    generator.playSound(context, SoundEnum::electric_buzz, 0.7F, 1.2F);
    context.effects
      .emitSparks(context.world, generator.position(), 16, Palette::spark_blue);

    context.noise.emit(
      generator.position(),
      GeneratorEntity::failure_noise_radius,
      NoiseSourceEnum::repair_failure
    );

    context.world.shake(0.4F);

    if(
      generator.progress()
      > 0.1F
      && context.world.random().chance(failure_breakdown_chance)
    ) {
      generator.triggerIncident(context, GeneratorIncidentEnum::breakdown);
      return;
    }

    context.hud.announce(
      Palette::danger,
      std::format(
        "SHORT CIRCUIT  -  GENERATOR {} LOST CHARGE",
        generator.label()
      )
    );
  }
} // namespace wheel_of_fear
