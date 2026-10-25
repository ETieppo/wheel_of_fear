#pragma once

#include "core/math/ColorProps.hpp"
#include <string>

namespace wheel_of_fear {
  struct CreditLineProps {
      std::string text;
      float       size          = 14;
      ColorProps  color         = ColorProps::white();
      bool        is_display    = false;
      float       spacing_after = 8;
  };
} // namespace wheel_of_fear
