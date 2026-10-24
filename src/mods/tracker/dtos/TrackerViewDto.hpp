#pragma once

#include <span>

#include "mods/tracker/SignalProps.hpp"

namespace wheel_of_fear {
  struct TrackerViewDto {
      float                        open_amount = 0;
      float                        sweep_angle = 0;
      std::span<const SignalProps> signals;
      std::span<const float>       last_ping_seconds;
  };
} // namespace wheel_of_fear
