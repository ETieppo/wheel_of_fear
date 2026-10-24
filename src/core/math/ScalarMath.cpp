#include "core/math/ScalarMath.hpp"

#include <cmath>

namespace wheel_of_fear {
  float ScalarMath::floorModulo(
    float value,
    float divisor
  ) {
    return value - (divisor * std::floor(value / divisor));
  }
} // namespace wheel_of_fear
