#pragma once

#include "core/math/ColorProps.hpp"
#include "core/math/Vec2Props.hpp"

namespace wheel_of_fear {
  struct LightProps {
      static constexpr float point_light_cone = -2;

      Vec2Props  position {};
      float      radius    = 0;
      float      intensity = 0;
      ColorProps color     = ColorProps::white();
      Vec2Props  direction {};
      float      cone_outer_cosine = point_light_cone;
      float      cone_inner_cosine = 0;
      bool       is_ultraviolet    = false;
      bool       casts_shadows     = true;
      float      flicker           = 0;
      bool       freezes_creatures = false;

      [[nodiscard]]
      bool isSpotlight() const {
        return cone_outer_cosine > -1.5F;
      }
  };
} // namespace wheel_of_fear
