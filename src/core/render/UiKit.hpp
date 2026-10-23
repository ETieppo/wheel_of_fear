#pragma once

#include <cstddef>
#include <string_view>

#include "core/math/Color.hpp"
#include "core/render/Renderer.hpp"

namespace wheel_of_fear {
  class UiKit final {
    public:
      UiKit() = delete;

      static
      void drawPanel(
        Renderer &renderer,
        float     x,
        float     y,
        float     width,
        float     height,
        Color     accent,
        float     alpha
      );
      static
      float drawKeycap(
        Renderer        &renderer,
        std::string_view label,
        float            x,
        float            y,
        float            size,
        Color            color
      );
      static
      void drawSegmentedBar(
        Renderer   &renderer,
        float       x,
        float       y,
        std::size_t segment_count,
        float       segment_width,
        float       height,
        float       fill,
        Color       color,
        Color       empty_color
      );
      static
      void drawGlitchTitle(
        Renderer        &renderer,
        std::string_view text,
        float            x,
        float            y,
        float            size,
        Color            color,
        float            seconds,
        float            intensity
      );
      static
      void drawScanlines(
        Renderer &renderer,
        float     width,
        float     height,
        float     alpha
      );
  };
} // namespace wheel_of_fear
