#pragma once

#include "core/math/Vec2Props.hpp"
#include "mods/mobs/MobEntity.hpp"
#include <memory>
#include <vector>

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
      static constexpr float                         hug_reach = 22;
      const std::vector<std::unique_ptr<MobEntity>> &mobs_;
  };
} // namespace wheel_of_fear
