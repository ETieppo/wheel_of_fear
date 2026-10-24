#pragma once

#include <cstddef>

#include "core/math/Vec2Props.hpp"

namespace wheel_of_fear {
  struct ScorchMarkProps {
      Vec2Props   position {};
      float       rotation = 0;
      std::size_t variant  = 0;
  };
} // namespace wheel_of_fear
