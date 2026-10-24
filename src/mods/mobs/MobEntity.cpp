#include "mods/mobs/MobEntity.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/time/GameClock.hpp"
#include "mods/effects/EffectSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include <algorithm>

namespace wheel_of_fear {
  MobEntity::MobEntity(
    Vec2Props             position,
    const MobTraitsProps &traits
  )
      : Entity(
          position,
          std::min(
            traits.collision_radius * 0.6F,
            5.0F
          )
        )
      , brain_(
          *this,
          position
        )
      , perception_(*this)
      , locomotion_(*this)
      , condition_(*this)
      , voice_(*this)
      , appearance_(*this) {
    setHeading(
      ScalarMath::floorModulo(
        (position.x * 0.37F) + (position.y * 0.11F),
        ScalarMath::tau
      )
    );
  }

  float MobEntity::distanceToPlayer(const GameContextDto &context) const {
    return distanceTo(context.player.position());
  }

  void MobEntity::update(GameContextDto &context) {
    if(!isAlive()) return;

    const float delta = context.clock.deltaSeconds();
    if(!condition_.isFrozen()) advanceAnimation(delta);

    incident_cooldown_ = std::max(0.0F, incident_cooldown_ - delta);
    brain_.tickCooldowns(delta);
    voice_.tickCooldown(delta);
    setMoving(false);
    think(context);
    voice_.playMovementSound(context);
  }

  void MobEntity::onContact(GameContextDto &context) {
    brain_.strikeAndRetreat(context, default_retreat_seconds);
  }

  void MobEntity::ignite(GameContextDto &context) {
    condition_.startBurning();

    voice_.playSound(
      context,
      SoundEnum::mob_burn,
      0.9F,
      context.world.random().range(0.9F, 1.1F)
    );
  }

  void MobEntity::catchBlast(GameContextDto &context) {
    setAlive(false);
    condition_.setBeingHugged(false);
    context.effects.emitCrumble(context.world, position(), 14);
  }
} // namespace wheel_of_fear
