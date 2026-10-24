#include "mods/tracker/TrackerGhostField.hpp"

#include "core/time/GameClock.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/tracker/TrackerConfig.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  void TrackerGhostField::update(
    GameContextDto &context
  ) {
    const float delta = context.clock.deltaSeconds();

    driftAndExpire(delta);
    maybeSpawnGhost(context, delta);
  }

  void TrackerGhostField::driftAndExpire(
    float delta_seconds
  ) {
    for(GhostEchoProps &ghost : ghosts_) {
      ghost.remaining_seconds -= delta_seconds;
      ghost.position = ghost.position.add(ghost.drift.scale(delta_seconds));
    }

    std::erase_if(ghosts_, [](const GhostEchoProps &ghost) {
      return ghost.remaining_seconds <= 0;
    });
  }

  void TrackerGhostField::maybeSpawnGhost(
    GameContextDto &context,
    float           delta_seconds
  ) {
    RandomGenerator &random = context.world.random();

    const float ghost_rate =
      0.05F + (0.02F * static_cast<float>(context.world.phase()));

    if(
      ghosts_.size() >=
      TrackerConfig::ghost_capacity ||
      !random.chance(delta_seconds * ghost_rate)
    )
      return;

    GhostEchoProps  ghost;
    const Vec2Props bearing = random.direction();

    ghost.position = context.player.position().add(
      bearing.scale(random.range(120, TrackerConfig::scan_range))
    );

    const Vec2Props drift_direction = random.direction();

    ghost.drift             = drift_direction.scale(random.range(4, 18));
    ghost.remaining_seconds = random.range(6, 14);
    ghosts_.push_back(ghost);
  }
} // namespace wheel_of_fear
