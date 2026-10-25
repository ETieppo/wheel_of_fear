#include "mods/mobs/components/MobBrainComponent.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/time/GameClock.hpp"
#include "mods/mobs/MobEntity.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include <algorithm>
#include <cmath>

namespace wheel_of_fear {
  bool MobBrainComponent::isHunting() const {
    return intent_
      == MobIntentEnum::chasing
      || intent_
      == MobIntentEnum::investigating
      || intent_
      == MobIntentEnum::lurking;
  }

  void MobBrainComponent::tickCooldowns(float delta_seconds) {
    strike_cooldown_ = std::max(0.0F, strike_cooldown_ - delta_seconds);
    retreat_seconds_ = std::max(0.0F, retreat_seconds_ - delta_seconds);
  }

  void MobBrainComponent::hunt(GameContextDto &context) {
    if(!owner_.canAct()) return;
    chooseIntent(context);
    pursueIntent(context);

    if(
      strike_cooldown_
      <= 0
      && owner_.perception()
           .touchesPlayer(context, owner_.traits().strike_reach)
    )
      owner_.onContact(context);
  }

  void MobBrainComponent::changeIntent(
    MobIntentEnum intent,
    Vec2Props     destination
  ) {
    if(intent_ != intent) intent_seconds_ = 0;
    intent_                          = intent;
    destination_                     = destination;
    progress_.stuck_seconds          = 0;
    progress_.last_progress_position = owner_.position();
  }

  void MobBrainComponent::startSearching(Vec2Props anchor) {
    search_anchor_  = anchor;
    linger_seconds_ = 0;
    changeIntent(MobIntentEnum::searching, anchor);
  }

  void MobBrainComponent::strikeAndRetreat(
    GameContextDto &context,
    float           retreat_seconds
  ) {
    strikePlayer(context);
    retreat_seconds_ = retreat_seconds;
  }

  void MobBrainComponent::exposeHiddenPlayer(GameContextDto &context) {
    PlayerEntity &player = context.player;
    if(!player.isHidden()) return;
    player.dragOutOfLocker(context);
    MobPerceptionComponent &perception = owner_.perception();

    perception.forgetLocker();
    perception.rememberPlayerAt(player.position());
    changeIntent(MobIntentEnum::chasing, player.position());
    strikePlayer(context);
  }

  void MobBrainComponent::chooseIntent(GameContextDto &context) {
    const MobPerceptionComponent &perception      = owner_.perception();
    const MobMemoryProps         &memory          = perception.memory();
    const Vec2Props               player_position = context.player.position();

    if(retreat_seconds_ > 0) {
      if(intent_ != MobIntentEnum::retreating)
        changeIntent(MobIntentEnum::retreating, owner_.position());
      return;
    }

    if(perception.canSeePlayerNow()) {
      if(
        intent_
        == MobIntentEnum::lurking
        && context.world.isBurningLightAt(player_position)
      )
        return;

      if(intent_ != MobIntentEnum::chasing) {
        changeIntent(MobIntentEnum::chasing, player_position);
        owner_.voice().vocalize(context, 1.0F);
      }

      return;
    }

    if(intent_ == MobIntentEnum::chasing || intent_ == MobIntentEnum::lurking) {
      if(memory.seconds_since_seen < sight_memory_seconds) return;
      startSearching(memory.last_seen_player.value_or(owner_.position()));
      return;
    }

    if(
      memory.known_locker_index
      && *memory.known_locker_index
      < context.world.lockers().size()
    ) {
      const Vec2Props entry
        = context.world.lockers()[*memory.known_locker_index].entryPoint();

      if(
        intent_
        != MobIntentEnum::investigating
        || destination_.distance(entry)
        > 1
      )
        changeIntent(MobIntentEnum::investigating, entry);

      return;
    }

    if(memory.seconds_since_noise == 0 && memory.noise_position) {
      const Vec2Props noise_position = *memory.noise_position;

      if(
        intent_
        != MobIntentEnum::investigating
        || noise_position.distance(destination_)
        > 24
      )
        owner_.voice().vocalize(context, 0.4F);

      changeIntent(MobIntentEnum::investigating, noise_position);
    }
  }

  void MobBrainComponent::pursueIntent(GameContextDto &context) {
    intent_seconds_ += context.clock.deltaSeconds();

    switch(intent_) {
      case MobIntentEnum::patrolling:    patrol(context); break;
      case MobIntentEnum::investigating: investigate(context); break;
      case MobIntentEnum::chasing:       chase(context); break;
      case MobIntentEnum::searching:     search(context); break;
      case MobIntentEnum::retreating:    fleeFromPlayer(context); break;
      case MobIntentEnum::lurking:       lurk(context); break;
      case MobIntentEnum::calling:       break;
    }

    trackProgress(context);
  }

  void MobBrainComponent::trackProgress(GameContextDto &context) {
    if(
      intent_
      == MobIntentEnum::lurking
      || intent_
      == MobIntentEnum::calling
      || linger_seconds_
      > 0
    ) {
      progress_.stuck_seconds = 0;
      return;
    }

    if(owner_.position().distance(progress_.last_progress_position) > 10) {
      progress_.last_progress_position = owner_.position();
      progress_.stuck_seconds          = 0;
      return;
    }

    progress_.stuck_seconds += context.clock.deltaSeconds();
    if(progress_.stuck_seconds < stuck_timeout_seconds) return;

    progress_.stuck_seconds = 0;
    owner_.perception().forgetLocker();
    patrol_room_.reset();

    if(intent_ != MobIntentEnum::chasing)
      changeIntent(MobIntentEnum::patrolling, owner_.position());
  }

  void MobBrainComponent::patrol(GameContextDto &context) {
    WorldSystem &world = context.world;
    if(!patrol_room_) choosePatrolRoom(context);
    if(linger_seconds_ > 0) {
      linger_seconds_ -= context.clock.deltaSeconds();
      lookAround(context);
      if(linger_seconds_ <= 0) patrol_room_.reset();

      return;
    }

    if(owner_.distanceTo(destination_) < arrival_distance) {
      linger_seconds_
        = world.random().range(minimum_linger_seconds, maximum_linger_seconds);

      return;
    }

    MobLocomotionComponent &locomotion = owner_.locomotion();

    locomotion.navigateTo(
      context,
      destination_,
      locomotion.currentSpeed() * patrol_speed_factor
    );
  }

  void MobBrainComponent::choosePatrolRoom(GameContextDto &context) {
    WorldSystem               &world = context.world;
    std::optional<std::size_t> current_room
      = world.map().roomIndexAt(owner_.position());

    if(!current_room) current_room = world.map().roomIndexAt(destination_);
    const float pressure = ScalarMath::clamp(
      0.35F + (0.08F * static_cast<float>(world.phase())),
      0,
      0.8F
    );

    const auto player_room = world.map().roomIndexAt(context.player.position());
    const std::optional<std::size_t> toward
      = world.random().chance(pressure) ? player_room : std::nullopt;
    const std::size_t room_index
      = world.randomPatrolRoom(current_room.value_or(0), toward);

    patrol_room_ = room_index;
    destination_ = world.map().randomFloorInRoom(
      world.random(),
      world.map().rooms()[room_index]
    );

    progress_.last_progress_position = owner_.position();
  }

  void MobBrainComponent::investigate(GameContextDto &context) {
    MobLocomotionComponent &locomotion = owner_.locomotion();

    if(owner_.distanceTo(destination_) > arrival_distance) {
      const bool is_reachable = locomotion.navigateTo(
        context,
        destination_,
        locomotion.currentSpeed() * investigate_speed_factor
      );
      if(!is_reachable) startSearching(owner_.position());

      return;
    }

    MobPerceptionComponent &perception = owner_.perception();

    if(perception.memory().known_locker_index) {
      const std::size_t locker_index = *perception.memory().known_locker_index;
      perception.forgetLocker();

      if(context.player.hiding().hiddenLockerIndex() == locker_index) {
        exposeHiddenPlayer(context);
        return;
      }
    }

    startSearching(destination_);
  }

  void MobBrainComponent::search(GameContextDto &context) {
    WorldSystem &world = context.world;

    if(intent_seconds_ > search_seconds) {
      patrol_room_.reset();
      changeIntent(MobIntentEnum::patrolling, owner_.position());
      return;
    }

    if(linger_seconds_ > 0) {
      linger_seconds_ -= context.clock.deltaSeconds();
      lookAround(context);
      return;
    }

    if(
      owner_.distanceTo(destination_)
      < arrival_distance
      || progress_.stuck_seconds
      > 1.2F
    ) {
      linger_seconds_        = world.random().range(0.4F, 1.2F);
      const Vec2Props wander = world.random().direction();
      const float     reach  = world.random().range(12, search_radius);

      destination_
        = world.map().nearestFloor(search_anchor_.add(wander.scale(reach)));

      return;
    }

    MobLocomotionComponent &locomotion = owner_.locomotion();

    locomotion.navigateTo(
      context,
      destination_,
      locomotion.currentSpeed() * search_speed_factor
    );
  }

  void MobBrainComponent::chase(GameContextDto &context) {
    WorldSystem            &world      = context.world;
    MobLocomotionComponent &locomotion = owner_.locomotion();
    const bool      can_see_player     = owner_.perception().canSeePlayerNow();
    const Vec2Props player_position    = context.player.position();

    const Vec2Props target = can_see_player
      ? player_position
      : owner_.perception().memory().last_seen_player.value_or(player_position);

    destination_ = target;

    if(
      can_see_player
      && world.isBurningLightAt(player_position)
      && owner_.traits().avoids_hazards
    ) {
      if(!locomotion.navigateTo(context, target, locomotion.currentSpeed()))
        changeIntent(MobIntentEnum::lurking, target);

      return;
    }

    if(
      can_see_player
      && owner_.distanceTo(player_position)
      < direct_chase_distance
    ) {
      if(!locomotion.moveAlong(
           context,
           owner_.directionTo(player_position),
           locomotion.currentSpeed()
         ))
        changeIntent(MobIntentEnum::lurking, target);

      return;
    }

    const Vec2Props direction = can_see_player
      ? world.navigation().directionToPlayer(world.map(), owner_.position())
      : world.navigation().directionToward(
          world.map(),
          owner_.position(),
          target,
          world.elapsedSeconds()
        );

    const Vec2Props fallback
      = direction.length() < 0.5F ? owner_.directionTo(target) : direction;

    if(!locomotion.moveAlong(context, fallback, locomotion.currentSpeed()))
      changeIntent(MobIntentEnum::lurking, target);
  }

  void MobBrainComponent::lurk(GameContextDto &context) {
    const bool      can_see_player  = owner_.perception().canSeePlayerNow();
    const Vec2Props player_position = context.player.position();

    owner_.turnToward(
      player_position.sub(owner_.position()).angle(),
      context.clock.deltaSeconds(),
      MobLocomotionComponent::turn_rate
    );

    if(!context.world.isBurningLightAt(player_position) && can_see_player) {
      changeIntent(MobIntentEnum::chasing, player_position);
      return;
    }

    if(intent_seconds_ > lurk_give_up_seconds && !can_see_player)
      startSearching(owner_.position());
  }

  void MobBrainComponent::lookAround(GameContextDto &context) {
    const float delta = context.clock.deltaSeconds();
    const float sweep = std::sin(owner_.animationSeconds() * 1.7F) * 1.2F;

    owner_.turnToward(
      owner_.heading() + (sweep * delta * 3),
      delta,
      MobLocomotionComponent::turn_rate
    );
  }

  void MobBrainComponent::fleeFromPlayer(GameContextDto &context) {
    MobLocomotionComponent &locomotion = owner_.locomotion();

    const Vec2Props away
      = owner_.position().sub(context.player.position()).normalized();

    locomotion.moveAlong(
      context,
      away,
      locomotion.currentSpeed() * retreat_speed_factor
    );
    if(retreat_seconds_ <= 0) startSearching(owner_.position());
  }

  void MobBrainComponent::strikePlayer(GameContextDto &context) {
    strike_cooldown_ = strike_cooldown_seconds;
    context.player.takeHit(context, owner_.position());
  }
} // namespace wheel_of_fear
