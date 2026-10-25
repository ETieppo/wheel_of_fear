#pragma once

#include "core/math/Vec2Props.hpp"

namespace wheel_of_fear {
  struct NeonDropletProps {
      Vec2Props position {};
      float     radius = 0;
      float     seed   = 0;
  };
} // namespace wheel_of_fear
