#include "mods/mobs/Mob.hpp"

#include <algorithm>
#include <cmath>

#include "core/audio/SoundPlayer.hpp"
#include "core/math/Scalar.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/effects/Effects.hpp"
#include "mods/noise/Noise.hpp"
#include "mods/player/Player.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {
  namespace {
    constexpr float patrol_speed_factor          = 0.42F;
    constexpr float investigate_speed_factor     = 0.75F;
    constexpr float search_speed_factor          = 0.38F;
    constexpr float retreat_speed_factor         = 0.95F;
    constexpr float frenzy_speed_multiplier      = 1.4F;
    constexpr float turn_rate                    = 8;
    constexpr float arrival_distance             = 12;
    constexpr float sight_memory_seconds         = 2.5F;
    constexpr float search_seconds               = 7;
    constexpr float search_radius                = 64;
    constexpr float minimum_linger_seconds       = 1.2F;
    constexpr float maximum_linger_seconds       = 4;
    constexpr float breathing_detection_distance = 34;
    constexpr float close_sense_distance         = 26;
    constexpr float movement_sound_distance      = 300;
    constexpr float occluded_sound_factor        = 0.45F;
    constexpr float strike_cooldown_seconds      = 1.1F;
    constexpr float default_retreat_seconds      = 1.2F;
    constexpr float stuck_timeout_seconds        = 3.5F;
    constexpr float lurk_give_up_seconds         = 6;
    constexpr float direct_chase_distance        = 72;
    constexpr float burn_duration_seconds        = 0.9F;
    constexpr float hug_break_distance           = 28;
    constexpr float hug_seconds                  = 0.9F;
    constexpr float crumble_duration_seconds     = 0.7F;
    constexpr float petrify_recovery_per_second  = 0.5F;
  } // namespace

  Mob::Mob(
    Vec2                  position,
    const MobTraitsProps &traits
  )
      : Entity(
          position,
          std::min(
            traits.collision_radius * 0.6F,
            5.0F
          )
        )
      , destination_(position) {
    setHeading(
      Scalar::floorModulo(position.x * 0.37F + position.y * 0.11F, Scalar::tau)
    );
    memory_.last_progress_position = position;
  }

  bool Mob::isHunting() const {
    return intent_ ==
           MobIntentEnum::chasing ||
           intent_ ==
           MobIntentEnum::investigating ||
           intent_ == MobIntentEnum::lurking;
  }

  float Mob::currentSpeed() const {
    const float base_speed = traits().speed;
    return frenzy_seconds_ > 0 ? base_speed * frenzy_speed_multiplier
                               : base_speed;
  }

  void Mob::frenzy(
    float seconds
  ) {
    frenzy_seconds_ = std::max(frenzy_seconds_, seconds);
  }

  void Mob::calmFrenzy(
    float delta_seconds
  ) {
    frenzy_seconds_ = std::max(0.0F, frenzy_seconds_ - delta_seconds);
  }

  float Mob::distanceToPlayer(
    const GameContextDto &context
  ) const {
    return distanceTo(context.player.position());
  }

  bool Mob::touchesPlayer(
    const GameContextDto &context,
    float                 reach
  ) const {
    return context.player.canBeTargeted() && distanceToPlayer(context) < reach;
  }

  void Mob::changeIntent(
    MobIntentEnum intent,
    Vec2          destination
  ) {
    if(intent_ != intent) intent_seconds_ = 0;
    intent_                        = intent;
    destination_                   = destination;
    memory_.stuck_seconds          = 0;
    memory_.last_progress_position = position();
  }

  void Mob::update(
    GameContextDto &context
  ) {
    if(!isAlive()) return;
    const float delta = context.clock.deltaSeconds();
    if(!is_frozen_) advanceAnimation(delta);
    incident_cooldown_ = std::max(0.0F, incident_cooldown_ - delta);
    strike_cooldown_   = std::max(0.0F, strike_cooldown_ - delta);
    vocal_cooldown_    = std::max(0.0F, vocal_cooldown_ - delta);
    retreat_seconds_   = std::max(0.0F, retreat_seconds_ - delta);
    setMoving(false);
    think(context);
    playMovementSound(context);
  }

  void Mob::onContact(
    GameContextDto &context
  ) {
    strikeAndRetreat(context, default_retreat_seconds);
  }

  void Mob::ignite(
    GameContextDto &context
  ) {
    burn_seconds_    = burn_duration_seconds;
    is_being_hugged_ = false;
    playSound(
      context,
      SoundEnum::mob_burn,
      0.9F,
      context.world.random().range(0.9F, 1.1F)
    );
  }

  void Mob::catchBlast(
    GameContextDto &context
  ) {
    setAlive(false);
    is_being_hugged_ = false;
    context.effects.emitCrumble(context.world, position(), 14);
  }

  void Mob::perceive(
    GameContextDto &context,
    bool            can_notice
  ) {
    const float delta = context.clock.deltaSeconds();
    memory_.seconds_since_seen += delta;
    memory_.seconds_since_noise += delta;
    can_see_player_ = false;
    if(!can_notice || isBusy()) return;
    if(canSeePlayer(context)) {
      can_see_player_            = true;
      memory_.last_seen_player   = context.player.position();
      memory_.seconds_since_seen = 0;
    }
    const auto heard = context.noise.loudestHeardAt(
      context.world.map(),
      position(),
      traits().hearing_factor
    );
    if(!heard) return;
    if(
      heard->event.source ==
      NoiseSourceEnum::breathing &&
      context.player.isHidden() &&
      heard->event.position.distance(position()) < breathing_detection_distance
    ) {
      exposeHiddenPlayer(context);
      return;
    }
    memory_.noise_position      = heard->event.position;
    memory_.seconds_since_noise = 0;
  }

  bool Mob::canSeePlayer(
    const GameContextDto &context
  ) const {
    const MobTraitsProps &mob_traits = traits();
    const Player         &player     = context.player;
    if(!player.canBeTargeted() || !mob_traits.canSee()) return false;
    const float distance = distanceToPlayer(context);
    if(distance > player.sightRangeForMobs() * mob_traits.sight_factor)
      return false;
    const bool in_view =
      distance <
      close_sense_distance ||
      isFacing(player.position(), mob_traits.field_of_view_cosine) ||
      player.flashlight().isOn();
    return in_view &&
           context.world.map().hasLineOfSight(position(), player.position());
  }

  void Mob::thinkAsHunter(
    GameContextDto &context
  ) {
    if(!canAct()) return;
    chooseIntent(context);
    pursueIntent(context);
    if(strike_cooldown_ <= 0 && touchesPlayer(context, traits().strike_reach))
      onContact(context);
  }

  void Mob::chooseIntent(
    GameContextDto &context
  ) {
    const Vec2 player_position = context.player.position();
    if(retreat_seconds_ > 0) {
      if(intent_ != MobIntentEnum::retreating)
        changeIntent(MobIntentEnum::retreating, position());
      return;
    }
    if(can_see_player_) {
      if(
        intent_ ==
        MobIntentEnum::lurking &&
        context.world.isHazardAt(player_position)
      )
        return;
      if(intent_ != MobIntentEnum::chasing) {
        changeIntent(MobIntentEnum::chasing, player_position);
        vocalize(context, 1.0F);
      }
      return;
    }
    if(intent_ == MobIntentEnum::chasing || intent_ == MobIntentEnum::lurking) {
      if(memory_.seconds_since_seen < sight_memory_seconds) return;
      startSearching(memory_.last_seen_player.value_or(position()));
      return;
    }
    if(
      memory_.known_locker_index &&
      *memory_.known_locker_index < context.world.lockers().size()
    ) {
      const Vec2 entry =
        context.world.lockers()[*memory_.known_locker_index].entryPoint();
      if(
        intent_ !=
        MobIntentEnum::investigating ||
        destination_.distance(entry) > 1
      )
        changeIntent(MobIntentEnum::investigating, entry);
      return;
    }
    if(memory_.seconds_since_noise == 0 && memory_.noise_position) {
      const Vec2 noise_position = *memory_.noise_position;
      if(
        intent_ !=
        MobIntentEnum::investigating ||
        noise_position.distance(destination_) > 24
      )
        vocalize(context, 0.4F);
      changeIntent(MobIntentEnum::investigating, noise_position);
    }
  }

  void Mob::pursueIntent(
    GameContextDto &context
  ) {
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

  void Mob::trackProgress(
    GameContextDto &context
  ) {
    if(
      intent_ ==
      MobIntentEnum::lurking ||
      intent_ ==
      MobIntentEnum::calling ||
      linger_seconds_ > 0
    ) {
      memory_.stuck_seconds = 0;
      return;
    }
    if(position().distance(memory_.last_progress_position) > 10) {
      memory_.last_progress_position = position();
      memory_.stuck_seconds          = 0;
      return;
    }
    memory_.stuck_seconds += context.clock.deltaSeconds();
    if(memory_.stuck_seconds < stuck_timeout_seconds) return;
    memory_.stuck_seconds = 0;
    memory_.known_locker_index.reset();
    patrol_room_.reset();
    if(intent_ != MobIntentEnum::chasing)
      changeIntent(MobIntentEnum::patrolling, position());
  }

  void Mob::patrol(
    GameContextDto &context
  ) {
    World &world = context.world;
    if(!patrol_room_) {
      std::optional<std::size_t> current_room =
        world.map().roomIndexAt(position());
      if(!current_room) current_room = world.map().roomIndexAt(destination_);
      const float pressure = Scalar::clamp(
        0.35F + 0.08F * static_cast<float>(world.phase()),
        0,
        0.8F
      );
      const auto player_room =
        world.map().roomIndexAt(context.player.position());
      const std::optional<std::size_t> toward =
        world.random().chance(pressure) ? player_room : std::nullopt;
      const std::size_t room_index =
        world.randomPatrolRoom(current_room.value_or(0), toward);
      patrol_room_ = room_index;
      destination_ = world.map().randomFloorInRoom(
        world.random(),
        world.map().rooms()[room_index]
      );
      memory_.last_progress_position = position();
    }
    if(linger_seconds_ > 0) {
      linger_seconds_ -= context.clock.deltaSeconds();
      lookAround(context);
      if(linger_seconds_ <= 0) patrol_room_.reset();
      return;
    }
    if(distanceTo(destination_) < arrival_distance) {
      linger_seconds_ =
        world.random().range(minimum_linger_seconds, maximum_linger_seconds);
      return;
    }
    navigateTo(context, destination_, currentSpeed() * patrol_speed_factor);
  }

  void Mob::investigate(
    GameContextDto &context
  ) {
    if(distanceTo(destination_) > arrival_distance) {
      if(!navigateTo(
           context,
           destination_,
           currentSpeed() * investigate_speed_factor
         ))
        startSearching(position());
      return;
    }
    if(memory_.known_locker_index) {
      const std::size_t locker_index = *memory_.known_locker_index;
      memory_.known_locker_index.reset();
      if(context.player.hiding().hiddenLockerIndex() == locker_index) {
        exposeHiddenPlayer(context);
        return;
      }
    }
    startSearching(destination_);
  }

  void Mob::startSearching(
    Vec2 anchor
  ) {
    search_anchor_  = anchor;
    linger_seconds_ = 0;
    changeIntent(MobIntentEnum::searching, anchor);
  }

  void Mob::search(
    GameContextDto &context
  ) {
    World &world = context.world;
    if(intent_seconds_ > search_seconds) {
      patrol_room_.reset();
      changeIntent(MobIntentEnum::patrolling, position());
      return;
    }
    if(linger_seconds_ > 0) {
      linger_seconds_ -= context.clock.deltaSeconds();
      lookAround(context);
      return;
    }
    if(
      distanceTo(destination_) <
      arrival_distance ||
      memory_.stuck_seconds > 1.2F
    ) {
      linger_seconds_    = world.random().range(0.4F, 1.2F);
      const Vec2  wander = world.random().direction();
      const float reach  = world.random().range(12, search_radius);
      destination_ =
        world.map().nearestFloor(search_anchor_.add(wander.scale(reach)));
      return;
    }
    navigateTo(context, destination_, currentSpeed() * search_speed_factor);
  }

  void Mob::chase(
    GameContextDto &context
  ) {
    World     &world           = context.world;
    const Vec2 player_position = context.player.position();
    const Vec2 target = can_see_player_
                        ? player_position
                        : memory_.last_seen_player.value_or(player_position);
    destination_      = target;
    if(
      can_see_player_ &&
      world.isHazardAt(player_position) &&
      traits().avoids_hazards
    ) {
      if(!navigateTo(context, target, currentSpeed()))
        changeIntent(MobIntentEnum::lurking, target);
      return;
    }
    if(can_see_player_ && distanceTo(player_position) < direct_chase_distance) {
      if(!moveAlong(context, directionTo(player_position), currentSpeed()))
        changeIntent(MobIntentEnum::lurking, target);
      return;
    }
    const Vec2 direction =
      can_see_player_
        ? world.navigation().directionToPlayer(world.map(), position())
        : world.navigation().directionToward(
            world.map(),
            position(),
            target,
            world.elapsedSeconds()
          );
    const Vec2 fallback =
      direction.length() < 0.5F ? directionTo(target) : direction;
    if(!moveAlong(context, fallback, currentSpeed()))
      changeIntent(MobIntentEnum::lurking, target);
  }

  void Mob::lurk(
    GameContextDto &context
  ) {
    const Vec2 player_position = context.player.position();
    turnToward(
      player_position.sub(position()).angle(),
      context.clock.deltaSeconds(),
      turn_rate
    );
    if(!context.world.isHazardAt(player_position) && can_see_player_) {
      changeIntent(MobIntentEnum::chasing, player_position);
      return;
    }
    if(intent_seconds_ > lurk_give_up_seconds && !can_see_player_)
      startSearching(position());
  }

  void Mob::lookAround(
    GameContextDto &context
  ) {
    const float delta = context.clock.deltaSeconds();
    const float sweep = std::sin(animationSeconds() * 1.7F) * 1.2F;
    turnToward(heading() + sweep * delta * 3, delta, turn_rate);
  }

  bool Mob::navigateTo(
    GameContextDto &context,
    Vec2            destination,
    float           speed
  ) {
    World     &world     = context.world;
    const Vec2 direction = world.navigation().directionToward(
      world.map(),
      position(),
      destination,
      world.elapsedSeconds()
    );
    if(direction.length() < 0.5F)
      return moveAlong(context, directionTo(destination), speed);
    return moveAlong(context, direction, speed);
  }

  bool Mob::moveAlong(
    GameContextDto &context,
    Vec2            direction,
    float           speed
  ) {
    if(direction.length() < 0.1F) return true;
    World      &world = context.world;
    const float delta = context.clock.deltaSeconds();
    const Vec2  step  = direction.scale(speed * delta);
    if(traits().avoids_hazards && !world.isHazardAt(position())) {
      const Vec2 probe =
        position().add(direction.scale(traits().collision_radius + 4));
      if(world.isHazardAt(probe)) {
        turnToward(direction.angle(), delta, turn_rate);
        return false;
      }
    }
    slide(world.map(), step);
    turnToward(direction.angle(), delta, turn_rate);
    return true;
  }

  void Mob::fleeFromPlayer(
    GameContextDto &context
  ) {
    const Vec2 away = position().sub(context.player.position()).normalized();
    moveAlong(context, away, currentSpeed() * retreat_speed_factor);
    if(retreat_seconds_ <= 0) startSearching(position());
  }

  void Mob::strikeAndRetreat(
    GameContextDto &context,
    float           retreat_seconds
  ) {
    strikePlayer(context);
    retreat_seconds_ = retreat_seconds;
  }

  void Mob::strikePlayer(
    GameContextDto &context
  ) {
    strike_cooldown_ = strike_cooldown_seconds;
    context.player.takeHit(context, position());
  }

  void Mob::exposeHiddenPlayer(
    GameContextDto &context
  ) {
    Player &player = context.player;
    if(!player.isHidden()) return;
    player.dragOutOfLocker(context);
    memory_.known_locker_index.reset();
    memory_.last_seen_player   = player.position();
    memory_.seconds_since_seen = 0;
    changeIntent(MobIntentEnum::chasing, player.position());
    strikePlayer(context);
  }

  void Mob::hearShepherd(
    Vec2 target
  ) {
    if(intent_ == MobIntentEnum::chasing) return;
    memory_.noise_position      = target;
    memory_.seconds_since_noise = 0;
  }

  void Mob::playSound(
    GameContextDto &context,
    SoundEnum       sound,
    float           volume,
    float           pitch
  ) const {
    const Vec2  listener = context.player.position();
    const float occlusion =
      context.world.map().hasLineOfSight(position(), listener)
        ? 1.0F
        : occluded_sound_factor;
    context.sound_player
      .playAt(sound, volume * occlusion, listener, position(), pitch);
  }

  void Mob::vocalize(
    GameContextDto &context,
    float           chance
  ) {
    const auto sound = traits().vocal_sound;
    if(!sound) return;
    RandomGenerator &random = context.world.random();
    if(vocal_cooldown_ > 0 || !random.chance(chance)) return;
    vocal_cooldown_ = random.range(3, 6);
    playSound(
      context,
      *sound,
      traits().vocal_volume,
      random.range(0.88F, 1.12F)
    );
  }

  void Mob::playMovementSound(
    GameContextDto &context
  ) {
    const MobTraitsProps &mob_traits = traits();
    if(!mob_traits.movement_sound) return;
    movement_sound_cooldown_ -= context.clock.deltaSeconds();
    if(!isMoving() || movement_sound_cooldown_ > 0) return;
    if(distanceToPlayer(context) > movement_sound_distance) return;
    RandomGenerator &random  = context.world.random();
    const float      urgency = intent_ == MobIntentEnum::chasing ? 0.7F : 1.0F;
    movement_sound_cooldown_ =
      mob_traits.movement_sound_interval * urgency * random.range(0.85F, 1.2F);
    playSound(
      context,
      *mob_traits.movement_sound,
      mob_traits.movement_sound_volume,
      random.range(0.9F, 1.12F)
    );
  }

  void Mob::advanceBurning(
    GameContextDto &context
  ) {
    const float delta = context.clock.deltaSeconds();
    World      &world = context.world;
    if(isBurning()) {
      burn_seconds_ -= delta;
      petrify_amount_ =
        1 - std::max(burn_seconds_, 0.0F) / burn_duration_seconds;
      if(world.random().chance(delta * 30))
        context.effects.emitSparks(world, position(), 2, Palette::neon_magenta);
      if(burn_seconds_ > 0) return;
      setAlive(false);
      world.addScorchMark(position());
      context.effects.emitCrumble(world, position(), 18);
      return;
    }
    if(isBusy() || !traits().can_burn || isDisguised()) return;
    if(!world.isHazardAt(position())) return;
    ignite(context);
    context.noise.emit(position(), 200, NoiseSourceEnum::mob_incident);
  }

  void Mob::advanceHugAndCrumble(
    GameContextDto &context
  ) {
    const float delta = context.clock.deltaSeconds();
    World      &world = context.world;
    if(isCrumbling()) {
      crumble_seconds_ -= delta;
      petrify_amount_ =
        1 + (1 - std::max(crumble_seconds_, 0.0F) / crumble_duration_seconds);
      if(world.random().chance(delta * 20))
        context.effects.emitCrumble(world, position(), 1);
      if(crumble_seconds_ > 0) return;
      setAlive(false);
      context.effects.emitCrumble(world, position(), 26);
      playSound(
        context,
        SoundEnum::crumble,
        0.9F,
        world.random().range(0.9F, 1.1F)
      );
      return;
    }
    if(!is_being_hugged_) {
      if(!isBurning())
        petrify_amount_ =
          std::max(0.0F, petrify_amount_ - delta * petrify_recovery_per_second);
      return;
    }
    if(
      distanceToPlayer(context) > hug_break_distance || context.player.isDead()
    ) {
      is_being_hugged_ = false;
      return;
    }
    if(petrify_amount_ == 0)
      context.sound_player.play(SoundEnum::crumble, 0.4F, 0, 0.6F);
    petrify_amount_ = std::min(1.0F, petrify_amount_ + delta / hug_seconds);
    if(petrify_amount_ < 1) return;
    is_being_hugged_ = false;
    crumble_seconds_ = crumble_duration_seconds;
    world.shake(0.25F);
  }

  Vec2 Mob::trembleOffset(
    float world_seconds
  ) const {
    if(!is_frozen_ || is_being_hugged_) return Vec2 {};
    return Vec2::of(
      (Scalar::hashTime(world_seconds + destination_.x, 30) - 0.5F) * 1.2F,
      (Scalar::hashTime(world_seconds + 3.1F, 30) - 0.5F) * 0.6F
    );
  }

  void Mob::drawBody(
    GameContextDto         &context,
    const AtlasRegionProps &body,
    const AtlasRegionProps &glow,
    const MobLookDto       &look
  ) const {
    const float presence_amount = presence();
    if(presence_amount <= 0.01F) return;
    Renderer  &renderer = context.renderer;
    const Vec2 draw_position =
      position().add(trembleOffset(context.clock.realSeconds()));
    const float rotation = heading() + look.rotation_offset;
    const Color tint     = isBurning() ? Color::hex(0xfff2d8) : look.tint;
    renderer.drawShadowDisc(
      draw_position.add(Vec2::of(1.5F, 3)),
      traits().collision_radius * 1.25F * look.scale,
      Color::black().withAlpha(0.5F * presence_amount),
      1.3F
    );
    renderer.drawSprite(
      body,
      draw_position,
      SpriteDrawDto {
        .rotation = rotation,
        .color    = tint.withAlpha(presence_amount),
        .petrify  = petrify_amount_,
        .glow     = isBurning() ? 0.8F : 0.0F,
        .scale    = look.scale
      }
    );
    if(petrify_amount_ > 0.5F) return;
    float glow_level = 0.75F;
    if(frenzy_seconds_ > 0 || intent_ == MobIntentEnum::chasing)
      glow_level = 1.0F;
    if(is_frozen_) glow_level = 0.35F;
    renderer.drawGlowSprite(
      glow,
      draw_position,
      look.glow_color.withAlpha(
        glow_level * look.glow_strength * std::max(presence_amount, 0.35F)
      ),
      SpriteDrawDto {.rotation = rotation, .scale = look.scale}
    );
  }
} // namespace wheel_of_fear
