#pragma once

#include "core/math/Vec2.hpp"

namespace wheel_of_fear {
  struct FlashlightBeamDto {
      Vec2  origin {};
      Vec2  direction {};
      float range             = 0;
      float cone_outer_cosine = 0;
  };
} // namespace wheel_of_fear
