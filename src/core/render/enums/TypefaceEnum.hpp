#pragma once

#include <cstddef>
#include <cstdint>

namespace wheel_of_fear {
  enum class TypefaceEnum : std::uint8_t {
    body,
    display,
  };

  inline constexpr std::size_t typeface_count = 2;
} // namespace wheel_of_fear
