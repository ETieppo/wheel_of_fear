#pragma once

#include <cstdint>
#include <string_view>

namespace wheel_of_fear {
  struct SpriteStripProps {
      std::string_view path;
      std::uint32_t    frame_count     = 1;
      std::uint32_t    frame_width     = 0;
      std::uint32_t    frame_height    = 0;
      float            pixels_per_unit = 8;
  };
} // namespace wheel_of_fear
