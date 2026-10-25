#pragma once

#include "core/math/Vec2Props.hpp"
#include <cstddef>
#include <optional>

namespace wheel_of_fear {
  struct MobMemoryProps {
      std::optional<Vec2Props>   last_seen_player;
      float                      seconds_since_seen = 1000;
      std::optional<Vec2Props>   noise_position;
      float                      seconds_since_noise = 1000;
      std::optional<std::size_t> known_locker_index;
      std::optional<Vec2Props>   lure_position;
  };
} // namespace wheel_of_fear
