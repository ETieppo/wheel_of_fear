#pragma once

#include <cmath>

#include "core/math/ColorProps.hpp"

namespace wheel_of_fear {
  struct LedGradientProps {
      ColorProps start_color = ColorProps::white();
      ColorProps end_color   = ColorProps::white();

      [[nodiscard]]
      ColorProps colorAt(float fraction_along) const {
        const float wrapped = fraction_along - std::floor(fraction_along);
        const float blend   = 1 - std::abs((wrapped * 2) - 1);

        return start_color.mix(end_color, blend);
      }
  };
} // namespace wheel_of_fear
