#pragma once

#include <cstdint>

namespace wheel_of_fear {
  enum class QuadKindEnum : std::uint8_t {
    sprite     = 0,
    solid      = 1,
    soft_disc  = 2,
    ring       = 3,
    glyph      = 4,
    crt_screen = 5,
    light_beam = 6,
  };
} // namespace wheel_of_fear
