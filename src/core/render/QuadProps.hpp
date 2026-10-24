#pragma once

#include <array>

namespace wheel_of_fear {
  struct QuadProps {
      std::array<float, 4> position_size {};
      std::array<float, 4> texture_region {};
      std::array<float, 4> color {};
      std::array<float, 4> parameters {};
  };

  static_assert(sizeof(QuadProps) == 64);
} // namespace wheel_of_fear
