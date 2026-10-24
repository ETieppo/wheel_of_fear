#pragma once

#include "core/render/Renderer.hpp"

namespace wheel_of_fear {
  class LockerHidingComponent;

  class HidingOverlayWidget final {
    public:
      HidingOverlayWidget() = delete;

      static void draw(
        Renderer                    &renderer,
        const LockerHidingComponent &hiding,
        float                        real_seconds
      );
  };
} // namespace wheel_of_fear
