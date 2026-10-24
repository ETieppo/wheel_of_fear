#pragma once

#include "core/math/Vec2Props.hpp"
#include <optional>

namespace wheel_of_fear {
  struct FlashlightGripDto {
      Vec2Props                holder_position {};
      Vec2Props                aim_direction {};
      std::optional<Vec2Props> repair_target;
  };
} // namespace wheel_of_fear
