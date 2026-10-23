#pragma once

#include "core/math/Color.hpp"
#include "core/math/Vec2.hpp"
#include "core/render/Renderer.hpp"

namespace wheel_of_fear {
  class Crosshair final {
    public:
      Crosshair() = delete;

      static
      void draw(
        Renderer &renderer,
        Vec2      mouse,
        Color     color,
        float     real_seconds
      );
  };
} // namespace wheel_of_fear
