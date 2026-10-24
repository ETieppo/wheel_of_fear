#pragma once

#include <cstdint>

namespace wheel_of_fear {
  enum class LightFilterEnum : std::uint8_t {
    any_light,
    ultraviolet_only,
  };
} // namespace wheel_of_fear
