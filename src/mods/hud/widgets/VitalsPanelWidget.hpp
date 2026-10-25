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

    private:
      static constexpr float health_pip_left        = 94;
      static constexpr float health_pip_spacing     = 30;
      static constexpr float health_pip_gap         = 6;
      static constexpr float health_pip_track_width = 144;
  };
} // namespace wheel_of_fear
