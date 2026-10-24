#pragma once

#include "core/render/GlyphProps.hpp"
#include <array>

namespace wheel_of_fear {
  struct FontProps {
      std::array<GlyphProps, 256> glyphs {};
      float                       pixel_size = 64;
  };
} // namespace wheel_of_fear
