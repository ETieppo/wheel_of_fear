#pragma once

#include <cstddef>
#include <cstdint>

namespace wheel_of_fear {
  enum class ItemEnum : std::uint8_t {
    gear,
    fuse,
    valve,
    lure_beacon,
    neon_grenade,
  };

  inline constexpr std::size_t item_count = 5;

  constexpr
  std::size_t indexOf(ItemEnum item) {
    return static_cast<std::size_t>(item);
  }
} // namespace wheel_of_fear
