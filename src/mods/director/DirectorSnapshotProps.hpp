#pragma once

#include <cstdint>

namespace wheel_of_fear {
  struct DirectorSnapshotProps {
      bool          is_in_phase = false;
      std::uint32_t phase       = 1;
      std::uint64_t phase_seed  = 0;
  };
} // namespace wheel_of_fear
