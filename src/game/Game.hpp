#pragma once

#include "core/audio/SoundPlayer.hpp"
#include "core/input/Input.hpp"
#include "core/platform/Window.hpp"
#include "core/render/Renderer.hpp"
#include "core/render/gpu/GpuContext.hpp"
#include "core/render/gpu/GpuRenderer.hpp"
#include "core/time/FrameTimer.hpp"
#include "core/time/GameClock.hpp"
#include "game/GameSnapshotProps.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/art/Art.hpp"
#include "mods/camera/Camera.hpp"
#include "mods/director/Director.hpp"
#include "mods/distractions/Distractions.hpp"
#include "mods/effects/Effects.hpp"
#include "mods/generators/Generators.hpp"
#include "mods/hud/Hud.hpp"
#include "mods/lighting/Lighting.hpp"
#include "mods/mobs/Mobs.hpp"
#include "mods/noise/Noise.hpp"
#include "mods/player/Player.hpp"
#include "mods/throwables/Throwables.hpp"
#include "mods/tracker/Tracker.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {
  class Game {
    public:
      explicit Game(Window &window);
      Game(const Game &)             = delete;
      Game &operator =(const Game &) = delete;

      bool              runFrame();
      GameSnapshotProps snapshot() const;
      void              resume(const GameSnapshotProps &snapshot);

    private:
      static constexpr float maximum_frame_seconds = 1.0F / 20.0F;

      bool pollEvents();
      void tick();
      void draw();

      Window        &window_;
      Renderer       renderer_;
      GpuContext     gpu_context_;
      GpuRenderer    gpu_renderer_;
      SoundPlayer    sound_player_;
      Input          input_;
      GameClock      clock_;
      FrameTimer     frame_timer_;
      Art            art_;
      World          world_;
      Lighting       lighting_;
      Player         player_;
      Mobs           mobs_;
      Generators     generators_;
      Effects        effects_;
      Tracker        tracker_;
      Hud            hud_;
      Noise          noise_;
      Throwables     throwables_;
      Distractions   distractions_;
      Camera         camera_;
      Director       director_;
      GameContextDto context_;
  };
} // namespace wheel_of_fear
