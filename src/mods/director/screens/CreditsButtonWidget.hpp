#pragma once

#include "core/math/RectProps.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/Renderer.hpp"

namespace wheel_of_fear {
  class CreditsButtonWidget final {
    public:
      CreditsButtonWidget() = delete;

      static constexpr RectProps bounds {
        .x      = RenderConfig::ui_width - 164,
        .y      = RenderConfig::ui_height - 52,
        .width  = 140,
        .height = 32,
      };

      static void draw(
        Renderer &renderer,
        bool      is_hovered
      );
  };
} // namespace wheel_of_fear
