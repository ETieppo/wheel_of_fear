#pragma once

#include "core/render/Renderer.hpp"

namespace wheel_of_fear {
  class PlayerEntity;

  class VitalsPanelWidget final {
    public:
      VitalsPanelWidget() = delete;

      static void draw(
        Renderer           &renderer,
        const PlayerEntity &player,
        float               real_seconds
      );
  };
} // namespace wheel_of_fear
