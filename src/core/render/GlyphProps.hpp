#pragma once

#include "core/render/AtlasRegionProps.hpp"

namespace wheel_of_fear {
  struct GlyphProps {
      AtlasRegionProps region {};
      float            bearing_x = 0;
      float            bearing_y = 0;
      float            advance   = 0;
  };
} // namespace wheel_of_fear
