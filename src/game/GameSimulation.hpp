#pragma once

#include "core/audio/SoundPlayer.hpp"
#include "core/input/InputController.hpp"
#include "core/math/Vec2Props.hpp"
#include "core/render/Renderer.hpp"
#include "core/time/GameClock.hpp"
#include "game/GameSnapshotProps.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/art/ArtLibrary.hpp"
#include "mods/camera/CameraSystem.hpp"
#include "mods/director/Director.hpp"
#include "mods/distractions/DistractionSystem.hpp"
#include "mods/effects/EffectSystem.hpp"
#include "mods/generators/GeneratorSystem.hpp"
#include "mods/hud/HudSystem.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/noise/NoiseSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/throwables/ThrowableSystem.hpp"
#include "mods/tracker/TrackerSystem.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  class GameSimulation {
    public:
      GameSimulation();
      GameSimulation(const GameSimulation &)             = delete;
      GameSimulation &operator =(const GameSimulation &) = delete;
      void            resume(const GameSnapshotProps &snapshot);

      [[nodiscard]]
      InputController &input() {
        return input_;
      }

      [[nodiscard]]
      Renderer &renderer() {
        return renderer_;
      }

      [[nodiscard]]
      const Renderer &renderer() const {
        return renderer_;
      }

      [[nodiscard]]
      GameContextDto &context() {
        return context_;
      }

      void step(float delta_seconds);
      [[nodiscard]]
      GameSnapshotProps snapshot() const;

    private:
      void tick();
      void draw();

      Renderer          renderer_;
      SoundPlayer       sound_player_;
      InputController   input_;
      GameClock         clock_;
      ArtLibrary        art_;
      WorldSystem       world_;
      LightingSystem    lighting_;
      PlayerEntity      player_;
      MobSystem         mobs_;
      GeneratorSystem   generators_;
      EffectSystem      effects_;
      TrackerSystem     tracker_;
      HudSystem         hud_;
      NoiseSystem       noise_;
      ThrowableSystem   throwables_;
      DistractionSystem distractions_;
      CameraSystem      camera_;
      Director          director_;
      GameContextDto    context_;
  };
} // namespace wheel_of_fear
