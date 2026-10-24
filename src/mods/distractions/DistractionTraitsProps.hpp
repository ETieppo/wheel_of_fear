#pragma once

#include <cstddef>
#include <optional>

#include "core/audio/enums/SoundEnum.hpp"
#include "core/math/ColorProps.hpp"

namespace wheel_of_fear {
  struct DistractionTraitsProps {
      SoundEnum                 sound        = SoundEnum::metal_clang;
      float                     volume       = 1;
      float                     pitch        = 1;
      float                     noise_radius = 0;
      std::optional<ColorProps> flash_color;
      float                     flash_radius  = 0;
      float                     flash_seconds = 0;
      std::size_t               spark_count   = 0;
  };
} // namespace wheel_of_fear
