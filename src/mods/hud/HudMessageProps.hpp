#pragma once

#include <string>

#include "core/math/Color.hpp"

namespace wheel_of_fear {
  struct HudMessageProps {
      std::string text;
      Color       color       = Color::white();
      float       age_seconds = 0;
  };
} // namespace wheel_of_fear
