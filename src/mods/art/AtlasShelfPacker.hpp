#pragma once

#include <cstdint>

#include "core/render/AtlasRegionProps.hpp"

namespace wheel_of_fear {
  class AtlasShelfPacker {
    public:
      AtlasShelfPacker(
        std::uint32_t width_limit,
        std::uint32_t height_limit
      )
          : width_limit_(width_limit)
          , height_limit_(height_limit) {
      }

      AtlasRegionProps reserve(
        std::uint32_t width,
        std::uint32_t height
      );

    private:
      static constexpr std::uint32_t padding = 2;

      std::uint32_t cursor_x_   = 4 + padding;
      std::uint32_t cursor_y_   = padding;
      std::uint32_t row_height_ = 0;
      std::uint32_t width_limit_;
      std::uint32_t height_limit_;
  };
} // namespace wheel_of_fear
