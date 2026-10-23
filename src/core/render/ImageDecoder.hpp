#pragma once

#include <filesystem>

#include "core/render/DecodedImageProps.hpp"

namespace wheel_of_fear {
  class ImageDecoder final {
    public:
      ImageDecoder() = delete;

      static
      DecodedImageProps decodeRgba(
        const std::filesystem::path &path,
        bool                         premultiply
      );
  };
} // namespace wheel_of_fear
