#pragma once

#include "core/render/Canvas.hpp"
#include "core/render/FontProps.hpp"
#include <cstdint>
#include <filesystem>

namespace wheel_of_fear {
  class FontBaker final {
    public:
      FontBaker() = delete;

      static std::uint32_t bake(
        const std::filesystem::path &font_path,
        FontProps                   &font,
        Canvas                       atlas,
        std::uint32_t                top
      );
  };
} // namespace wheel_of_fear
