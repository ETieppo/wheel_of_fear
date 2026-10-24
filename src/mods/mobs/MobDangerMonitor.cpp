#include "mods/mobs/MobDangerMonitor.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/time/GameClock.hpp"
#include "mods/player/PlayerEntity.hpp"
#include <algorithm>
#include <limits>

namespace wheel_of_fear {
  void MobDangerMonitor::update(GameContextDto &context) {
    const float nearest = nearestHunterDistance(context.player.position());
    const float delta   = context.clock.deltaSeconds();
    const float target_level
      = ScalarMath::clamp(1 - (nearest / danger_distance), 0, 1);

    level_ = ScalarMath::approach(level_, target_level, delta * 2);
    if(level_ < 0.35F) return;

    playHeartbeat(context, delta);
  }

  float
  MobDangerMonitor::nearestHunterDistance(Vec2Props player_position) const {
    float nearest = std::numeric_limits<float>::infinity();

    for(const auto &mob : mobs_) {
      if(
        !mob->traits().is_hostile
        || !mob->canAct()
        || !mob->brain().isHunting()
        || mob->isDisguised()
      )
        continue;

      nearest = std::min(nearest, mob->distanceTo(player_position));
    }

    return nearest;
  }

  void MobDangerMonitor::playHeartbeat(
    GameContextDto &context,
    float           delta_seconds
  ) {
    heartbeat_timer_ -= delta_seconds;
    if(heartbeat_timer_ > 0) return;

    heartbeat_timer_ = 0.95F - (level_ * 0.45F);
    context.sound_player
      .play(SoundEnum::heartbeat, 0.4F + (level_ * 0.5F), 0, 1);
  }
} // namespace wheel_of_fear
