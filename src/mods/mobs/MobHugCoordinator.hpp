#pragma once

#include <memory>
#include <vector>

#include "core/math/Vec2Props.hpp"
#include "mods/mobs/MobEntity.hpp"

namespace wheel_of_fear {
  class MobHugCoordinator {
    public:
      explicit MobHugCoordinator(
        const std::vector<std::unique_ptr<MobEntity>> &mobs
      )
          : mobs_(mobs) {
      }

      [[nodiscard]]
      bool isHugInProgress() const;
      bool tryStartHug(
        Vec2Props position,
        Vec2Props facing
      );
      void cancelHug();

    private:
      static constexpr float hug_reach = 22;

      const std::vector<std::unique_ptr<MobEntity>> &mobs_;
  };
} // namespace wheel_of_fear
