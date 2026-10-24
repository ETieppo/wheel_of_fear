#pragma once

#include "core/render/DecodedImageProps.hpp"
#include <array>
#include <cstddef>

namespace wheel_of_fear {
  inline constexpr std::size_t ground_texture_count = 6;

  struct GroundMaterialsDto {
      DecodedImageProps floor_color;
      DecodedImageProps floor_normal;
      DecodedImageProps wall_color;
      DecodedImageProps wall_normal;
      DecodedImageProps face_color;
      DecodedImageProps face_normal;

      std::array<
        DecodedImageProps*,
        ground_texture_count
      >
      inBindingOrder() {
        return {
          &floor_color,
          &floor_normal,
          &wall_color,
          &wall_normal,
          &face_color,
          &face_normal,
        };
      }
  };
} // namespace wheel_of_fear
