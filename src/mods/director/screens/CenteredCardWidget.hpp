#pragma once

#include <string_view>

#include "core/math/ColorProps.hpp"
#include "core/render/Renderer.hpp"

namespace wheel_of_fear {
  class CenteredCardWidget final {
    public:
      CenteredCardWidget() = delete;

      static void draw(
        Renderer        &renderer,
        std::string_view title,
        std::string_view subtitle,
        ColorProps       color,
        float            real_seconds
      );
  };
} // namespace wheel_of_fear
