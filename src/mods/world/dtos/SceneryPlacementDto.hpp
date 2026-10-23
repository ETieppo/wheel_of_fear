#pragma once

#include <cstddef>

#include "core/math/Color.hpp"
#include "core/math/Vec2.hpp"

namespace wheel_of_fear {
  struct SceneryPlacementDto {
      Vec2        position {};
      bool        mirrored     = false;
      std::size_t room_index   = 0;
      Color       color        = Color::white();
      float       flicker_seed = 0;
  };
} // namespace wheel_of_fear
