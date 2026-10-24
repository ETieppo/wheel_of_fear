#pragma once

#include <cstdint>

namespace wheel_of_fear {
  enum class GeneratorIncidentEnum : std::uint8_t {
    none,
    jam,
    breakdown,
  };
} // namespace wheel_of_fear
