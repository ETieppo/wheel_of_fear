#pragma once

#include "core/math/ColorProps.hpp"
#include "core/render/Renderer.hpp"
#include "core/render/SegmentedBarProps.hpp"
#include <cstddef>
#include <string_view>

namespace wheel_of_fear {
  class UiPainter final {
    public:
      UiPainter() = delete;

      static void drawPanel(
        Renderer  &renderer,
        float      x,
        float      y,
        float      width,
        float      height,
        ColorProps accent,
        float      alpha
      );

      static float drawKeycap(
        Renderer        &renderer,
        std::string_view label,
        float            x,
        float            y,
        float            size,
        ColorProps       color
      );

      static void drawSegmentedBar(
        Renderer                &renderer,
        const SegmentedBarProps &bar
      );

      static void drawGlitchTitle(
        Renderer        &renderer,
        std::string_view text,
        float            x,
        float            y,
        float            size,
        ColorProps       color,
        float            seconds,
        float            intensity
      );

      static void drawScanlines(
        Renderer &renderer,
        float     width,
        float     height,
        float     alpha
      );
  };
} // namespace wheel_of_fear
