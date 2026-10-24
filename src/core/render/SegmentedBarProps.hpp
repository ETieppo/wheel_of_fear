#pragma once

#include "core/math/ColorProps.hpp"
#include <cstddef>

namespace wheel_of_fear {
  struct SegmentedBarProps {
      float       x             = 0;
      float       y             = 0;
      std::size_t segment_count = 0;
      float       segment_width = 0;
      float       height        = 0;
      float       fill          = 0;
      ColorProps  color         = {};
      ColorProps  empty_color   = {};
  };
} // namespace wheel_of_fear
