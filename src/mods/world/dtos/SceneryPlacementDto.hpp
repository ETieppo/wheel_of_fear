#pragma once

#include "core/math/ColorProps.hpp"
#include "core/math/Vec2Props.hpp"
#include <cstddef>

namespace wheel_of_fear {
  struct SceneryPlacementDto {
      Vec2Props   position {};
      bool        mirrored     = false;
      std::size_t room_index   = 0;
      ColorProps  color        = ColorProps::white();
      float       flicker_seed = 0;
  };
} // namespace wheel_of_fear
