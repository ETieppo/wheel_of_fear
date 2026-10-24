#pragma once

#include "core/math/ColorProps.hpp"
#include "core/math/Vec2Props.hpp"
#include <cstddef>

namespace wheel_of_fear {
  struct ParticleProps {
      Vec2Props   position {};
      Vec2Props   velocity {};
      float       remaining_seconds = 0;
      float       lifetime_seconds  = 1;
      float       rotation          = 0;
      float       spin              = 0;
      float       size              = 1;
      ColorProps  color             = ColorProps::white();
      std::size_t variant           = 0;
  };
} // namespace wheel_of_fear
