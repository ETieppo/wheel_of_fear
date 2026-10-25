#pragma once

#include "core/math/Vec2Props.hpp"
#include <cstddef>
#include <memory>
#include <span>

namespace wheel_of_fear {
  class MobEntity;
  class PlayerEntity;
  class TileMapModel;

  class MobBodyCollisionResolver final {
    public:
      MobBodyCollisionResolver() = delete;

      static constexpr float body_radius_factor = 0.8F;

      static void resolve(
        std::span<const std::unique_ptr<MobEntity>> mobs,
        PlayerEntity                               &player,
        const TileMapModel                         &map
      );

      [[nodiscard]]
      static float bodyRadius(const MobEntity &mob);

    private:
      static constexpr int resolve_passes = 2;

      [[nodiscard]]
      static bool hasBody(const MobEntity &mob);

      [[nodiscard]]
      static bool isPinned(const MobEntity &mob);

      [[nodiscard]]
      static Vec2Props separationDirection(
        Vec2Props   from,
        Vec2Props   to,
        std::size_t tie_breaker
      );

      static void separateMobs(
        MobEntity          &first,
        MobEntity          &second,
        std::size_t         tie_breaker,
        const TileMapModel &map
      );

      static void separateFromPlayer(
        MobEntity          &mob,
        PlayerEntity       &player,
        const TileMapModel &map
      );
  };
} // namespace wheel_of_fear
