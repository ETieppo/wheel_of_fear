#pragma once

#include "mods/world/NeonDropletProps.hpp"
#include <vector>

namespace wheel_of_fear {
  struct NeonSplashProps {
      std::vector<NeonDropletProps> droplets;
      float                         lifetime_seconds = 0;
  };
} // namespace wheel_of_fear
