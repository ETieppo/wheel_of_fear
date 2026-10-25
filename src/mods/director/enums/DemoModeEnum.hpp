#pragma once

#include <cstdint>

namespace wheel_of_fear {
  enum class DemoModeEnum : std::uint8_t {
    none,
    play,
    deep,
    tracker,
    sectors,
    showcase,
    freeze,
    moth,
    locker,
    witness,
    repair,
    phase,
    power,
    grenade,
    lure,
    doors,
    walk,
    ending,
  };
} // namespace wheel_of_fear
