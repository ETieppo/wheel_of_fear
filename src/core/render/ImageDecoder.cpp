#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG

#include "core/render/ImageDecoder.hpp"
#include "core/assets/AssetLocator.hpp"
#include <cstddef>
#include <stb_image.h>
#include <stdexcept>

namespace wheel_of_fear {
  DecodedImageProps ImageDecoder::decodeRgba(
    const std::filesystem::path &path,
    bool                         premultiply
  ) {
    const auto bytes    = AssetLocator::readBytes(path);
    int        width    = 0;
    int        height   = 0;
    int        channels = 0;

    stbi_uc* decoded = stbi_load_from_memory(
      bytes.data(),
      static_cast<int>(bytes.size()),
      &width,
      &height,
      &channels,
      4
    );

    if(decoded == nullptr)
      throw std::runtime_error("unable to decode image " + path.string());

    DecodedImageProps image;
    image.width  = static_cast<std::uint32_t>(width);
    image.height = static_cast<std::uint32_t>(height);
    image.pixels.assign(
      decoded,
      decoded
        + (static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4)
    );
    stbi_image_free(decoded);

    if(premultiply) {
      for(std::size_t index = 0; index < image.pixels.size(); index += 4) {
        const std::uint32_t alpha = image.pixels[index + 3];

        for(std::size_t channel = 0; channel < 3; ++channel) {
          image.pixels[index + channel] = static_cast<std::uint8_t>(
            ((image.pixels[index + channel] * alpha) + 127) / 255
          );
        }
      }
    }

    return image;
  }
} // namespace wheel_of_fear
