#pragma once

#include "core/math/ColorProps.hpp"
#include <string>

namespace wheel_of_fear {
  struct HudMessageProps {
      std::string text;
      ColorProps  color       = ColorProps::white();
      float       age_seconds = 0;
  };
} // namespace wheel_of_fear
