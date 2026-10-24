#pragma once

#include "game/dtos/GameContextDto.hpp"
#include "mods/mobs/MobEntity.hpp"
#include <memory>
#include <vector>

namespace wheel_of_fear {
  class MobDangerMonitor {
    public:
      explicit MobDangerMonitor(
        const std::vector<std::unique_ptr<MobEntity>> &mobs
      )
          : mobs_(mobs) {
      }

      [[nodiscard]]
      float level() const {
        return level_;
      }

      void reset() {
        level_ = 0;
      }

      void update(GameContextDto &context);

    private:
      static constexpr float danger_distance = 170;

      [[nodiscard]]
      float nearestHunterDistance(Vec2Props player_position) const;

      void playHeartbeat(
        GameContextDto &context,
        float           delta_seconds
      );

      const std::vector<std::unique_ptr<MobEntity>> &mobs_;
      float                                          level_           = 0;
      float                                          heartbeat_timer_ = 0;
  };
} // namespace wheel_of_fear
