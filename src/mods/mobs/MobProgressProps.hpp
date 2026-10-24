#pragma once

#include "core/math/Vec2Props.hpp"

namespace wheel_of_fear {
  struct MobProgressProps {
      float     stuck_seconds = 0;
      Vec2Props last_progress_position {};
  };
} // namespace wheel_of_fear
