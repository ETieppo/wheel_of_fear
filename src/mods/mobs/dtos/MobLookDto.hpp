#pragma once

#include "core/math/ColorProps.hpp"

namespace wheel_of_fear {
  struct MobLookDto {
      ColorProps tint            = ColorProps::white();
      ColorProps glow_color      = ColorProps::white();
      float      scale           = 1;
      float      rotation_offset = 0;
      float      glow_strength   = 1;
  };
} // namespace wheel_of_fear
