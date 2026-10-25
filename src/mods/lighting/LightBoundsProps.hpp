#pragma once

#include "core/math/Vec2Props.hpp"

namespace wheel_of_fear {
  struct LightBoundsProps {
      Vec2Props minimum {};
      Vec2Props maximum {};

      [[nodiscard]]
      bool contains(Vec2Props point) const {
        return point.x
          >= minimum.x
          && point.y
          >= minimum.y
          && point.x
          <= maximum.x
          && point.y
          <= maximum.y;
      }
  };
} // namespace wheel_of_fear
