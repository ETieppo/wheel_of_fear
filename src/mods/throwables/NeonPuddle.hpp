#pragma once

#include "core/math/Vec2.hpp"
#include "mods/lighting/Lighting.hpp"

namespace wheel_of_fear {
  class NeonPuddle {
    public:
      static constexpr float lifetime_seconds = 60;

      explicit NeonPuddle(
        Vec2 position
      )
          : position_(position) {
      }

      bool isDry() const {
        return remaining_seconds_ <= 0;
      }

      void update(
        float delta_seconds
      );
      void emitLights(
        Lighting &lighting,
        float     real_seconds
      ) const;

    private:
      static constexpr float flash_duration_seconds = 0.5F;
      static constexpr float light_radius           = 95;

      Vec2  position_;
      float remaining_seconds_ = lifetime_seconds;
      float flash_seconds_     = flash_duration_seconds;
  };
} // namespace wheel_of_fear
