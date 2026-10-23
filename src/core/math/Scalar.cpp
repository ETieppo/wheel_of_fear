#include "core/math/Scalar.hpp"

#include <cmath>

namespace wheel_of_fear {
  float Scalar::floorModulo(
    float value,
    float divisor
  ) {
    return value - divisor * std::floor(value / divisor);
  }
} // namespace wheel_of_fear
