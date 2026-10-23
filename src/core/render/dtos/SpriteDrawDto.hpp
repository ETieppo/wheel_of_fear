#pragma once

#include "core/math/Color.hpp"

namespace wheel_of_fear {
  struct SpriteDrawDto {
      float rotation = 0;
      Color color    = Color::white();
      bool  mirrored = false;
      float petrify  = 0;
      float glow     = 0;
      float scale    = 1;
  };
} // namespace wheel_of_fear
