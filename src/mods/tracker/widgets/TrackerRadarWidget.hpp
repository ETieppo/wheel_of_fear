#pragma once

#include <span>

#include "core/math/Vec2Props.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/tracker/SignalProps.hpp"
#include "mods/tracker/dtos/TrackerViewDto.hpp"

namespace wheel_of_fear {
  class TrackerRadarWidget final {
    public:
      TrackerRadarWidget() = delete;

      static void draw(
        GameContextDto       &context,
        const TrackerViewDto &view
      );

    private:
      static constexpr float blip_fade_seconds = 1.6F;

      static Vec2Props worldToRadar(
        Vec2Props offset
      );
      static void drawScope(
        GameContextDto &context,
        float           sweep_angle
      );
      static void drawWallEchoes(
        GameContextDto &context
      );
      static void drawPlayerMarker(
        GameContextDto &context
      );
      static void drawCaptions(
        GameContextDto &context,
        std::size_t     signal_count
      );
      static void drawSignal(
        GameContextDto    &context,
        const SignalProps &signal,
        float              last_ping
      );
      static void drawOutOfRangeSignal(
        GameContextDto    &context,
        const SignalProps &signal,
        Vec2Props          offset
      );
      static void drawSignalMarker(
        GameContextDto    &context,
        const SignalProps &signal,
        Vec2Props          point,
        float              glow
      );
  };
} // namespace wheel_of_fear
