#pragma once

#include "core/math/Vec2Props.hpp"

namespace wheel_of_fear {
  struct FlashlightBeamDto {
      Vec2Props origin {};
      Vec2Props direction {};
      float     range             = 0;
      float     cone_outer_cosine = 0;
  };
} // namespace wheel_of_fear
