#pragma once

#include "core/math/Vec2Props.hpp"

namespace wheel_of_fear {
  struct GhostEchoProps {
      Vec2Props position {};
      Vec2Props drift {};
      float     remaining_seconds = 0;
  };
} // namespace wheel_of_fear
