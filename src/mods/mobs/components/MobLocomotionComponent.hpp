#pragma once

#include "core/math/Vec2Props.hpp"
#include "game/dtos/GameContextDto.hpp"

namespace wheel_of_fear {
  class MobEntity;

  class MobLocomotionComponent {
    public:
      static constexpr float frenzy_speed_multiplier = 1.25F;

      static constexpr float turn_rate = 8;

      explicit MobLocomotionComponent(
        MobEntity &owner
      )
          : owner_(owner) {
      }

      [[nodiscard]]
      float currentSpeed() const;
      bool  navigateTo(
        GameContextDto &context,
        Vec2Props       destination,
        float           speed
      );
      bool moveAlong(
        GameContextDto &context,
        Vec2Props       direction,
        float           speed
      );

    private:
      MobEntity &owner_;
  };
} // namespace wheel_of_fear
