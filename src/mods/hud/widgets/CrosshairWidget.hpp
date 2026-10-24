#pragma once

#include "core/math/ColorProps.hpp"
#include "core/math/Vec2Props.hpp"
#include "core/render/Renderer.hpp"

namespace wheel_of_fear {
  class CrosshairWidget final {
    public:
      CrosshairWidget() = delete;

      static void draw(
        Renderer  &renderer,
        Vec2Props  mouse,
        ColorProps color,
        float      real_seconds
      );
  };
} // namespace wheel_of_fear
