#pragma once

#include "core/math/Vec2Props.hpp"

namespace wheel_of_fear {
  class Collider {
    public:
      virtual ~Collider() = default;

      [[nodiscard]]
      virtual Vec2Props slide(
        Vec2Props position,
        Vec2Props displacement,
        float     half_size
      ) const = 0;
  };
} // namespace wheel_of_fear
