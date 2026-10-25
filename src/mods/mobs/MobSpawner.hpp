#pragma once

#include "core/math/Vec2Props.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/mobs/MobEntity.hpp"
#include "mods/mobs/enums/MobKindEnum.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

namespace wheel_of_fear {
  class MobSpawner {
    public:
      explicit MobSpawner(std::vector<std::unique_ptr<MobEntity>> &mobs)
          : mobs_(mobs) {
      }

      void spawn(
        MobKindEnum kind,
        Vec2Props   position
      );

      void populatePhase(WorldSystem &world);

      void update(GameContextDto &context);

    private:
      static constexpr int   minimum_spawn_grid_distance = 2;
      static constexpr float minimum_spawn_view_distance = 280;
      static constexpr int   mimic_placement_attempts    = 24;

      static std::optional<MobKindEnum> pickSpawnKind(WorldSystem &world);

      static float spawnIntervalSeconds(std::uint32_t phase);

      static std::size_t populationCap(std::uint32_t phase);

      static std::vector<std::size_t>
      distantRoomsFromStart(const WorldSystem &world);

      void spawnHostiles(
        WorldSystem                    &world,
        const std::vector<std::size_t> &distant_rooms
      );

      void spawnMimics(
        WorldSystem                    &world,
        const std::vector<std::size_t> &distant_rooms
      );

      void spawnMoth(WorldSystem &world);

      [[nodiscard]]
      std::size_t hostileCount() const;
      void        spawnFromVent(GameContextDto &context);

      std::vector<std::unique_ptr<MobEntity>> &mobs_;
      float                                    spawn_timer_ = 0;
  };
} // namespace wheel_of_fear
