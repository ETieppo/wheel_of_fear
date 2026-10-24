#include "mods/mobs/components/MobPerceptionComponent.hpp"

#include "core/time/GameClock.hpp"
#include "mods/flashlight/FlashlightComponent.hpp"
#include "mods/mobs/MobEntity.hpp"
#include "mods/noise/NoiseSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  void MobPerceptionComponent::perceive(
    GameContextDto &context,
    bool            can_notice
  ) {
    const float delta = context.clock.deltaSeconds();

    memory_.seconds_since_seen += delta;
    memory_.seconds_since_noise += delta;
    can_see_player_ = false;
    if(!can_notice || owner_.isBusy()) return;

    if(canSeePlayer(context)) {
      can_see_player_ = true;
      rememberPlayerAt(context.player.position());
    }

    listen(context);
  }

  void MobPerceptionComponent::listen(
    GameContextDto &context
  ) {
    const auto heard = context.noise.loudestHeardAt(
      context.world.map(),
      owner_.position(),
      owner_.traits().hearing_factor
    );
    if(!heard) return;

    if(
      heard->event.source ==
      NoiseSourceEnum::breathing &&
      context.player.isHidden() &&
      heard->event.position.distance(owner_.position()) <
      breathing_detection_distance
    ) {
      owner_.brain().exposeHiddenPlayer(context);
      return;
    }

    memory_.noise_position      = heard->event.position;
    memory_.seconds_since_noise = 0;
  }

  void MobPerceptionComponent::observeHiding(
    const GameContextDto &context,
    std::size_t           locker_index,
    Vec2Props             locker_position
  ) {
    const MobTraitsProps &mob_traits = owner_.traits();

    if(
      owner_.isBusy() ||
      !mob_traits.is_hostile ||
      !mob_traits.canSee() ||
      owner_.isDisguised()
    )
      return;

    const float distance = owner_.distanceTo(locker_position);

    const bool was_tracking = owner_.brain().intent() ==
                              MobIntentEnum::chasing &&
                              distance < chasing_witness_distance;

    const bool is_watching =
      distance < witness_distance && owner_.isFacing(locker_position, 0.1F);
    if(!was_tracking && !is_watching) return;

    if(!context.world.map().hasLineOfSight(owner_.position(), locker_position))
      return;

    witnessHiding(locker_index);
  }

  void MobPerceptionComponent::hearShepherd(
    Vec2Props target
  ) {
    if(owner_.brain().intent() == MobIntentEnum::chasing) return;

    memory_.noise_position      = target;
    memory_.seconds_since_noise = 0;
  }

  bool MobPerceptionComponent::touchesPlayer(
    const GameContextDto &context,
    float                 reach
  ) const {
    return context.player.canBeTargeted() &&
           owner_.distanceToPlayer(context) < reach;
  }

  bool MobPerceptionComponent::canSeePlayer(
    const GameContextDto &context
  ) const {
    const MobTraitsProps &mob_traits = owner_.traits();
    const PlayerEntity   &player     = context.player;
    if(!player.canBeTargeted() || !mob_traits.canSee()) return false;

    const float distance = owner_.distanceToPlayer(context);

    if(distance > player.sightRangeForMobs() * mob_traits.sight_factor)
      return false;

    const bool in_view =
      distance <
      close_sense_distance ||
      owner_.isFacing(player.position(), mob_traits.field_of_view_cosine) ||
      player.flashlight().isOn();

    return in_view && context.world.map().hasLineOfSight(
                        owner_.position(),
                        player.position()
                      );
  }
} // namespace wheel_of_fear
