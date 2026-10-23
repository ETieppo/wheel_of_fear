#pragma once

#include <cstddef>

namespace wheel_of_fear {
  enum class MusicLayerEnum {
    title,
    ambience,
    tension
  };

  inline constexpr std::size_t music_layer_count = 3;
} // namespace wheel_of_fear
