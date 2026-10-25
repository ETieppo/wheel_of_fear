#pragma once

#include "core/math/ColorProps.hpp"
#include "mods/world/NeonDropletProps.hpp"
#include <vector>

namespace wheel_of_fear {
  struct NeonSplashProps {
      std::vector<NeonDropletProps> droplets;
      float                         lifetime_seconds = 0;
      ColorProps                    color            = ColorProps::white();
  };
} // namespace wheel_of_fear
