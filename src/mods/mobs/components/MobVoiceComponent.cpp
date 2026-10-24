#include "mods/mobs/components/MobVoiceComponent.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/time/GameClock.hpp"
#include "mods/mobs/MobEntity.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include <algorithm>

namespace wheel_of_fear {
  void MobVoiceComponent::tickCooldown(float delta_seconds) {
    vocal_cooldown_ = std::max(0.0F, vocal_cooldown_ - delta_seconds);
  }

  void MobVoiceComponent::playSound(
    GameContextDto &context,
    SoundEnum       sound,
    float           volume,
    float           pitch
  ) const {
    const Vec2Props listener = context.player.position();
    const float     occlusion
      = context.world.map().hasLineOfSight(owner_.position(), listener)
      ? 1.0F
      : occluded_sound_factor;

    context.sound_player
      .playAt(sound, volume * occlusion, listener, owner_.position(), pitch);
  }

  void MobVoiceComponent::vocalize(
    GameContextDto &context,
    float           chance
  ) {
    const auto sound = owner_.traits().vocal_sound;
    if(!sound) return;

    RandomGenerator &random = context.world.random();
    if(vocal_cooldown_ > 0 || !random.chance(chance)) return;

    vocal_cooldown_ = random.range(3, 6);
    playSound(
      context,
      *sound,
      owner_.traits().vocal_volume,
      random.range(0.88F, 1.12F)
    );
  }

  void MobVoiceComponent::playMovementSound(GameContextDto &context) {
    const MobTraitsProps &mob_traits = owner_.traits();
    if(!mob_traits.movement_sound) return;

    movement_sound_cooldown_ -= context.clock.deltaSeconds();
    if(!owner_.isMoving() || movement_sound_cooldown_ > 0) return;
    if(owner_.distanceToPlayer(context) > movement_sound_distance) return;

    RandomGenerator &random = context.world.random();
    const float      urgency
      = owner_.brain().intent() == MobIntentEnum::chasing ? 0.7F : 1.0F;

    movement_sound_cooldown_ = mob_traits.movement_sound_interval
      * urgency
      * random.range(0.85F, 1.2F);

    playSound(
      context,
      *mob_traits.movement_sound,
      mob_traits.movement_sound_volume,
      random.range(0.9F, 1.12F)
    );
  }
} // namespace wheel_of_fear
