#pragma once

#include "core/math/Vec2Props.hpp"

namespace wheel_of_fear {
  struct RectProps {
      float x      = 0;
      float y      = 0;
      float width  = 0;
      float height = 0;

      [[nodiscard]]
      constexpr bool contains(Vec2Props point) const {
        return point.x
          >= x
          && point.x
          <= x
          + width
          && point.y
          >= y
          && point.y
          <= y
          + height;
      }
  };
} // namespace wheel_of_fear
