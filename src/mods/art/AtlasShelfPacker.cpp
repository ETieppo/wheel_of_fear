#include "mods/art/AtlasShelfPacker.hpp"

#include <algorithm>
#include <stdexcept>

namespace wheel_of_fear {
  AtlasRegionProps AtlasShelfPacker::reserve(
    std::uint32_t width,
    std::uint32_t height
  ) {
    if(cursor_x_ + width + padding > width_limit_) {
      cursor_x_ = padding;
      cursor_y_ += row_height_ + padding;
      row_height_ = 0;
    }
    if(cursor_y_ + height + padding > height_limit_)
      throw std::runtime_error("sprite atlas is full");
    const AtlasRegionProps region {
      static_cast<float>(cursor_x_),
      static_cast<float>(cursor_y_),
      static_cast<float>(width),
      static_cast<float>(height),
      1
    };
    cursor_x_ += width + padding;
    row_height_ = std::max(row_height_, height);
    return region;
  }
} // namespace wheel_of_fear
