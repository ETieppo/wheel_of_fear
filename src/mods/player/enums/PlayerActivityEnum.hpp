#pragma once

#include <cstdint>

namespace wheel_of_fear {
  enum class PlayerActivityEnum : std::uint8_t {
    exploring,
    repairing,
    hiding,
    hugging,
  };
} // namespace wheel_of_fear
