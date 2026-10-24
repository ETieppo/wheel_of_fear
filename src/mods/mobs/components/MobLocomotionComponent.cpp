#include "mods/mobs/components/MobLocomotionComponent.hpp"

#include "core/time/GameClock.hpp"
#include "mods/mobs/MobEntity.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  float MobLocomotionComponent::currentSpeed() const {
    const float base_speed = owner_.traits().speed;

    return owner_.condition().frenzySeconds() > 0
           ? base_speed * frenzy_speed_multiplier
           : base_speed;
  }

  bool MobLocomotionComponent::navigateTo(
    GameContextDto &context,
    Vec2Props       destination,
    float           speed
  ) {
    WorldSystem &world = context.world;

    const Vec2Props direction = world.navigation().directionToward(
      world.map(),
      owner_.position(),
      destination,
      world.elapsedSeconds()
    );

    if(direction.length() < 0.5F)
      return moveAlong(context, owner_.directionTo(destination), speed);

    return moveAlong(context, direction, speed);
  }

  bool MobLocomotionComponent::moveAlong(
    GameContextDto &context,
    Vec2Props       direction,
    float           speed
  ) {
    if(direction.length() < 0.1F) return true;

    WorldSystem          &world      = context.world;
    const MobTraitsProps &mob_traits = owner_.traits();
    const float           delta      = context.clock.deltaSeconds();
    const Vec2Props       step       = direction.scale(speed * delta);

    if(mob_traits.avoids_hazards && !world.isHazardAt(owner_.position())) {
      const Vec2Props probe =
        owner_.position().add(direction.scale(mob_traits.collision_radius + 4));

      if(world.isHazardAt(probe)) {
        owner_.turnToward(direction.angle(), delta, turn_rate);
        return false;
      }
    }

    owner_.slide(world.map(), step);
    owner_.turnToward(direction.angle(), delta, turn_rate);

    return true;
  }
} // namespace wheel_of_fear
