#pragma once

#include <cstdint>

namespace wheel_of_fear {
  enum class SignalKindEnum : std::uint8_t {
    generator,
    fabricator,
    exit,
    unidentified,
  };
} // namespace wheel_of_fear
