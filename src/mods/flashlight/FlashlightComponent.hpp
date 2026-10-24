#pragma once

#include "core/audio/SoundPlayer.hpp"
#include "core/math/ColorProps.hpp"
#include "core/math/RandomGenerator.hpp"
#include "core/render/Renderer.hpp"
#include "mods/art/SpriteSheetProps.hpp"
#include "mods/flashlight/FlashlightBatteryComponent.hpp"
#include "mods/flashlight/dtos/FlashlightBeamDto.hpp"
#include "mods/flashlight/dtos/FlashlightConditionsDto.hpp"
#include "mods/flashlight/dtos/FlashlightGripDto.hpp"
#include "mods/flashlight/enums/FlashlightModeEnum.hpp"
#include "mods/lighting/LightingSystem.hpp"

namespace wheel_of_fear {
  class FlashlightComponent {
    public:
      void reset();

      [[nodiscard]]
      FlashlightModeEnum mode() const {
        return mode_;
      }

      [[nodiscard]]
      bool isEnabled() const {
        return is_enabled_;
      }

      [[nodiscard]]
      bool isOn() const {
        return is_on_;
      }

      [[nodiscard]]
      const FlashlightBatteryComponent &battery() const {
        return battery_;
      }

      [[nodiscard]]
      bool isUltraviolet() const {
        return mode_ == FlashlightModeEnum::ultraviolet;
      }

      void setEnabled(bool is_enabled) {
        is_enabled_ = is_enabled;
      }

      void cycleMode(SoundPlayer &sound_player);
      void update(
        const FlashlightConditionsDto &conditions,
        RandomGenerator               &random,
        SoundPlayer                   &sound_player
      );
      void emitLights(
        LightingSystem          &lighting,
        const FlashlightGripDto &grip
      ) const;

      void drawBeam(
        Renderer                &renderer,
        const SpriteSheetProps  &sprites,
        const FlashlightGripDto &grip
      ) const;

      [[nodiscard]]
      float sightRangeForMobs() const;

      static Vec2Props lampTip(
        Vec2Props holder_position,
        Vec2Props aim_direction
      );

    private:
      static constexpr float lamp_forward_offset = 9;
      static constexpr float lamp_side_offset    = 3;
      static constexpr float repair_pool_radius  = 42;

      [[nodiscard]]
      bool usesUltraviolet(const FlashlightGripDto &grip) const;

      [[nodiscard]]
      ColorProps beamColor(const FlashlightGripDto &grip) const;

      [[nodiscard]]
      float beamIntensity(const FlashlightGripDto &grip) const;

      [[nodiscard]]
      FlashlightBeamDto beamShape(const FlashlightGripDto &grip) const;

      FlashlightModeEnum         mode_       = FlashlightModeEnum::normal;
      bool                       is_enabled_ = true;
      bool                       is_on_      = true;
      FlashlightBatteryComponent battery_;
  };
} // namespace wheel_of_fear
