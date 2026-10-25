#include "mods/mobs/components/MobConditionComponent.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/effects/EffectSystem.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/mobs/MobEntity.hpp"
#include "mods/noise/NoiseSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/throwables/ThrowableSystem.hpp"
#include "mods/world/WorldSystem.hpp"
#include <algorithm>

namespace wheel_of_fear {
  void MobConditionComponent::frenzy(float seconds) {
    frenzy_seconds_ = std::max(frenzy_seconds_, seconds);
  }

  void MobConditionComponent::calmFrenzy(float delta_seconds) {
    frenzy_seconds_ = std::max(0.0F, frenzy_seconds_ - delta_seconds);
  }

  bool
  MobConditionComponent::isHeldByLight(const GameContextDto &context) const {
    if(owner_.isDisguised()) return false;
    const auto filter = owner_.traits().frozen_by;

    if(!filter) return false;
    return context.lighting
             .freezingLightAt(context.world.map(), owner_.position(), *filter)
      > freeze_threshold;
  }

  void MobConditionComponent::startBurning() {
    burn_seconds_    = burn_duration_seconds;
    is_being_hugged_ = false;
  }

  void MobConditionComponent::advanceBurning(GameContextDto &context) {
    const float  delta = context.clock.deltaSeconds();
    WorldSystem &world = context.world;

    if(isBurning()) {
      burn_seconds_ -= delta;
      petrify_amount_
        = 1 - (std::max(burn_seconds_, 0.0F) / burn_duration_seconds);

      if(world.random().chance(delta * 30))
        context.effects.emitSparks(
          world,
          owner_.position(),
          2,
          context.throwables.grenadeColor()
        );
      if(burn_seconds_ > 0) return;

      owner_.setAlive(false);
      world.addScorchMark(owner_.position());
      context.effects.emitCrumble(world, owner_.position(), 18);

      return;
    }

    if(owner_.isBusy() || !owner_.traits().can_burn || owner_.isDisguised())
      return;
    if(!world.isHazardAt(owner_.position())) return;

    owner_.ignite(context);
    context.noise.emit(owner_.position(), 200, NoiseSourceEnum::mob_incident);
  }

  void MobConditionComponent::advanceHugAndCrumble(GameContextDto &context) {
    const float delta = context.clock.deltaSeconds();

    if(isCrumbling()) {
      advanceCrumbling(context, delta);
      return;
    }

    if(!is_being_hugged_) {
      recoverFromPetrify(delta);
      return;
    }

    if(
      owner_.distanceToPlayer(context)
      > hug_break_distance
      || context.player.isDead()
    ) {
      is_being_hugged_ = false;
      return;
    }

    if(petrify_amount_ == 0)
      context.sound_player.play(SoundEnum::crumble, 0.4F, 0, 0.6F);

    petrify_amount_ = std::min(1.0F, petrify_amount_ + (delta / hug_seconds));
    if(petrify_amount_ < 1) return;

    is_being_hugged_ = false;
    crumble_seconds_ = crumble_duration_seconds;
    context.world.shake(0.25F);
  }

  void MobConditionComponent::advanceCrumbling(
    GameContextDto &context,
    float           delta_seconds
  ) {
    WorldSystem &world = context.world;
    crumble_seconds_ -= delta_seconds;
    petrify_amount_
      = 1 + (1 - (std::max(crumble_seconds_, 0.0F) / crumble_duration_seconds));

    if(world.random().chance(delta_seconds * 20))
      context.effects.emitCrumble(world, owner_.position(), 1);
    if(crumble_seconds_ > 0) return;

    owner_.setAlive(false);
    context.effects.emitCrumble(world, owner_.position(), 26);
    owner_.voice().playSound(
      context,
      SoundEnum::crumble,
      0.9F,
      world.random().range(0.9F, 1.1F)
    );
  }

  void MobConditionComponent::recoverFromPetrify(float delta_seconds) {
    if(isBurning()) return;

    petrify_amount_ = std::max(
      0.0F,
      petrify_amount_ - (delta_seconds * petrify_recovery_per_second)
    );
  }
} // namespace wheel_of_fear
