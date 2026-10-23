#pragma once

#include "core/math/Color.hpp"

namespace wheel_of_fear {
  struct MobLookDto {
      Color tint            = Color::white();
      Color glow_color      = Color::white();
      float scale           = 1;
      float rotation_offset = 0;
      float glow_strength   = 1;
  };
} // namespace wheel_of_fear
