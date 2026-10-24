#pragma once

#include "core/math/Vec2Props.hpp"
#include "mods/noise/enums/NoiseSourceEnum.hpp"

namespace wheel_of_fear {
  struct NoiseEventProps {
      Vec2Props       position {};
      float           radius            = 0;
      NoiseSourceEnum source            = NoiseSourceEnum::footstep;
      float           remaining_seconds = 0;
  };
} // namespace wheel_of_fear
