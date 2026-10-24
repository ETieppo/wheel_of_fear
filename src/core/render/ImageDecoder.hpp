#pragma once

#include "core/render/DecodedImageProps.hpp"
#include <filesystem>

namespace wheel_of_fear {
  class ImageDecoder final {
    public:
      ImageDecoder() = delete;

      static DecodedImageProps decodeRgba(
        const std::filesystem::path &path,
        bool                         premultiply
      );
  };
} // namespace wheel_of_fear
