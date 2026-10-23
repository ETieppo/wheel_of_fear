#pragma once

#include "core/math/Vec2.hpp"

namespace wheel_of_fear {
  class Collider {
    public:
      virtual ~Collider() = default;

      virtual
      Vec2 slide(
        Vec2  position,
        Vec2  displacement,
        float half_size
      ) const = 0;
  };
} // namespace wheel_of_fear
