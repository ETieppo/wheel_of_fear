#pragma once

#include "core/math/Vec2Props.hpp"

namespace wheel_of_fear {
  struct RoomFillLightProps {
      Vec2Props position       = {};
      float     radius         = 0;
      float     distance_along = 0;
  };
} // namespace wheel_of_fear
