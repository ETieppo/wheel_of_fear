#pragma once

#include <cstddef>
#include <cstdint>

namespace wheel_of_fear {
  enum class MusicLayerEnum : std::uint8_t {
    title,
    ambience,
    tension,
  };

  inline constexpr std::size_t music_layer_count = 3;
} // namespace wheel_of_fear
