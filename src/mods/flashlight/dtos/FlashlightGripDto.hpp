#pragma once

#include <optional>

#include "core/math/Vec2.hpp"

namespace wheel_of_fear {
  struct FlashlightGripDto {
      Vec2                holder_position {};
      Vec2                aim_direction {};
      std::optional<Vec2> repair_target;
  };
} // namespace wheel_of_fear
