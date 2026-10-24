#pragma once

#include "core/platform/Window.hpp"
#include "core/render/gpu/GpuContext.hpp"
#include "core/render/gpu/GpuRenderer.hpp"
#include "core/time/FrameTimer.hpp"
#include "game/GameSimulation.hpp"
#include "game/GameSnapshotProps.hpp"

namespace wheel_of_fear {
  class GameApplication {
    public:
      explicit GameApplication(
        Window &window
      );
      GameApplication(
        const GameApplication &
      ) = delete;
      GameApplication &operator =(
        const GameApplication &
      ) = delete;

      bool runFrame();
      [[nodiscard]]
      GameSnapshotProps snapshot() const;
      void              resume(
        const GameSnapshotProps &snapshot
      );

    private:
      static constexpr float maximum_frame_seconds = 1.0F / 20.0F;

      bool pollEvents();

      Window        &window_;
      GpuContext     gpu_context_;
      GpuRenderer    gpu_renderer_;
      FrameTimer     frame_timer_;
      GameSimulation simulation_;
  };
} // namespace wheel_of_fear
