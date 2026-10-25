#pragma once

#include "core/math/ColorProps.hpp"
#include "core/math/Vec2Props.hpp"
#include "mods/lighting/LightingSystem.hpp"

namespace wheel_of_fear {
  class NeonPuddleHazard {
    public:
      static constexpr float lifetime_seconds = 90;

      NeonPuddleHazard(
        Vec2Props  position,
        ColorProps color
      )
          : position_(position)
          , color_(color) {
      }

      [[nodiscard]]
      Vec2Props position() const {
        return position_;
      }

      [[nodiscard]]
      bool isDry() const {
        return remaining_seconds_ <= 0;
      }

      void update(float delta_seconds);

      void emitLights(
        LightingSystem &lighting,
        float           real_seconds
      ) const;

    private:
      static constexpr float flash_duration_seconds = 0.5F;
      static constexpr float light_radius           = 95;

      Vec2Props  position_;
      ColorProps color_;
      float      remaining_seconds_ = lifetime_seconds;
      float      flash_seconds_     = flash_duration_seconds;
  };
} // namespace wheel_of_fear
