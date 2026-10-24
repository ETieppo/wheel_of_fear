#pragma once

#include "core/entity/Entity.hpp"
#include "core/math/Vec2.hpp"
#include "game/dtos/GameContextDto.hpp"

namespace wheel_of_fear {
  class PlayerMovement {
    public:
      bool isRunning() const {
        return is_running_;
      }

      float strideDistance() const {
        return stride_distance_;
      }

      void reset();

      void push(
        Vec2 impulse
      ) {
        knockback_ = impulse;
      }

      void update(
        GameContextDto &context,
        Entity         &body
      );

    private:
      static constexpr float walk_speed           = 60;
      static constexpr float run_speed            = 98;
      static constexpr float walking_noise_radius = 70;
      static constexpr float running_noise_radius = 270;
      static constexpr float tracker_slowdown     = 0.85F;

      void stepFootsteps(
        GameContextDto &context,
        const Entity   &body,
        float           travelled
      );

      Vec2  knockback_ {};
      float stride_distance_ = 0;
      float footstep_timer_  = 0;
      bool  is_running_      = false;
  };
} // namespace wheel_of_fear
