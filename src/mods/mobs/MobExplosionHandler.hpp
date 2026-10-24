#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "core/math/Vec2Props.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/mobs/MobEntity.hpp"

namespace wheel_of_fear {
  class MobExplosionHandler {
    public:
      explicit MobExplosionHandler(
        const std::vector<std::unique_ptr<MobEntity>> &mobs
      )
          : mobs_(mobs) {
      }

      std::size_t destroyInBlast(
        GameContextDto &context,
        Vec2Props       center,
        float           radius
      );
      void detonate(
        GameContextDto &context,
        MobEntity      &bomb
      );

    private:
      static constexpr float explosion_radius        = 52;
      static constexpr float explosion_player_radius = 44;
      static constexpr float explosion_noise_radius  = 520;

      static void announceExplosion(
        GameContextDto &context,
        Vec2Props       center
      );
      static void breakSurroundingWalls(
        GameContextDto &context,
        Vec2Props       center
      );
      void blastNearbyMobs(
        GameContextDto &context,
        Vec2Props       center
      );

      const std::vector<std::unique_ptr<MobEntity>> &mobs_;
  };
} // namespace wheel_of_fear
