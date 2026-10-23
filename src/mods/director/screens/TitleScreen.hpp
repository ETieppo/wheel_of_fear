#pragma once

#include "core/render/Renderer.hpp"

namespace wheel_of_fear {
  class TitleScreen final {
    public:
      TitleScreen() = delete;

      static
      void draw(
        Renderer &renderer,
        float     real_seconds
      );
  };
} // namespace wheel_of_fear
