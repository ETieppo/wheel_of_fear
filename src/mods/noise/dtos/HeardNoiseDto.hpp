#pragma once

#include "mods/noise/NoiseEventProps.hpp"

namespace wheel_of_fear {
  struct HeardNoiseDto {
      NoiseEventProps event {};
      float           loudness = 0;
  };
} // namespace wheel_of_fear
