#include "mods/mobs/MobBodyCollisionResolver.hpp"
#include "core/math/ScalarMath.hpp"
#include "mods/mobs/MobEntity.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/TileMapModel.hpp"

namespace wheel_of_fear {
  void MobBodyCollisionResolver::resolve(
    std::span<const std::unique_ptr<MobEntity>> mobs,
    PlayerEntity                               &player,
    const TileMapModel                         &map
  ) {
    const bool player_has_body = !player.isDead() && !player.isHidden();

    for(int pass = 0; pass < resolve_passes; ++pass) {
      for(std::size_t first = 0; first < mobs.size(); ++first) {
        if(!hasBody(*mobs[first])) continue;

        for(std::size_t second = first + 1; second < mobs.size(); ++second)
          if(hasBody(*mobs[second]))
            separateMobs(*mobs[first], *mobs[second], first + second, map);

        if(player_has_body) separateFromPlayer(*mobs[first], player, map);
      }
    }
  }

  float MobBodyCollisionResolver::bodyRadius(const MobEntity &mob) {
    return mob.traits().collision_radius * body_radius_factor;
  }

  bool MobBodyCollisionResolver::hasBody(const MobEntity &mob) {
    return mob.isAlive() && mob.traits().has_body_collision;
  }

  bool MobBodyCollisionResolver::isPinned(const MobEntity &mob) {
    return mob.condition().isFrozen();
  }

  Vec2Props MobBodyCollisionResolver::separationDirection(
    Vec2Props   from,
    Vec2Props   to,
    std::size_t tie_breaker
  ) {
    const Vec2Props offset = to.sub(from);
    if(offset.length() > 0.01F) return offset.normalized();

    return Vec2Props::fromAngle(
      static_cast<float>(tie_breaker) * ScalarMath::tau / 7
    );
  }

  void MobBodyCollisionResolver::separateMobs(
    MobEntity          &first,
    MobEntity          &second,
    std::size_t         tie_breaker,
    const TileMapModel &map
  ) {
    const float spacing  = bodyRadius(first) + bodyRadius(second);
    const float distance = first.position().distance(second.position());
    if(distance >= spacing) return;

    const bool is_first_pinned  = isPinned(first);
    const bool is_second_pinned = isPinned(second);
    if(is_first_pinned && is_second_pinned) return;

    const Vec2Props direction
      = separationDirection(first.position(), second.position(), tie_breaker);
    const float overlap = spacing - distance;

    if(is_first_pinned) {
      second.slide(map, direction.scale(overlap));
      return;
    }

    if(is_second_pinned) {
      first.slide(map, direction.scale(-overlap));
      return;
    }

    first.slide(map, direction.scale(-overlap / 2));
    second.slide(map, direction.scale(overlap / 2));
  }

  void MobBodyCollisionResolver::separateFromPlayer(
    MobEntity          &mob,
    PlayerEntity       &player,
    const TileMapModel &map
  ) {
    const float spacing  = bodyRadius(mob) + player.collisionHalfSize();
    const float distance = mob.position().distance(player.position());
    if(distance >= spacing) return;

    const Vec2Props direction
      = separationDirection(player.position(), mob.position(), 0);
    const float overlap = spacing - distance;

    if(isPinned(mob)) {
      player.slide(map, direction.scale(-overlap));
      return;
    }

    mob.slide(map, direction.scale(overlap));
  }
} // namespace wheel_of_fear
