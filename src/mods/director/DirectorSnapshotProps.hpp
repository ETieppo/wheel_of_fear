#pragma once

#include "mods/player/components/VitalityComponent.hpp"
#include <cstdint>

namespace wheel_of_fear {
  struct DirectorSnapshotProps {
      bool          is_in_phase       = false;
      std::uint32_t phase             = 1;
      std::uint64_t phase_seed        = 0;
      std::int32_t  player_max_health = VitalityComponent::base_max_health;
  };
} // namespace wheel_of_fear
