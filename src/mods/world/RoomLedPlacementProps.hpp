#pragma once

#include "core/math/Vec2Props.hpp"

namespace wheel_of_fear {
  struct RoomLedPlacementProps {
      float     distance_along       = 0;
      Vec2Props position             = {};
      Vec2Props outward              = {};
      bool      faces_open_passage   = false;
      bool      is_covered_by_object = false;
  };
} // namespace wheel_of_fear
