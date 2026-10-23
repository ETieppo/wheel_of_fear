#pragma once

#include "core/math/Vec2.hpp"

namespace wheel_of_fear {
  struct GhostEchoProps {
      Vec2  position {};
      Vec2  drift {};
      float remaining_seconds = 0;
  };
} // namespace wheel_of_fear
