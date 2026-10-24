#pragma once

#include <array>

namespace wheel_of_fear {
  struct GpuLightProps {
      std::array<float, 4> position_radius {};
      std::array<float, 4> direction_cone {};
      std::array<float, 4> color {};
      std::array<float, 4> animation {};
  };

  static_assert(
    sizeof(GpuLightProps) == 64
  );
} // namespace wheel_of_fear
