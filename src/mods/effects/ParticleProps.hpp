#pragma once

#include <cstddef>

#include "core/math/Color.hpp"
#include "core/math/Vec2.hpp"

namespace wheel_of_fear {
  struct ParticleProps {
      Vec2        position {};
      Vec2        velocity {};
      float       remaining_seconds = 0;
      float       lifetime_seconds  = 1;
      float       rotation          = 0;
      float       spin              = 0;
      float       size              = 1;
      Color       color             = Color::white();
      std::size_t variant           = 0;
  };
} // namespace wheel_of_fear
