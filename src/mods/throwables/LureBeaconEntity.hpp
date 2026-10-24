#pragma once

#include "core/math/Vec2Props.hpp"
#include "core/render/Renderer.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/art/SpriteSheetProps.hpp"
#include "mods/lighting/LightingSystem.hpp"

namespace wheel_of_fear {
  class LureBeaconEntity {
    public:
      explicit LureBeaconEntity(
        Vec2Props position
      )
          : position_(position) {
      }

      [[nodiscard]]
      Vec2Props position() const {
        return position_;
      }

      [[nodiscard]]
      bool isExpired() const {
        return remaining_seconds_ <= 0;
      }

      void update(
        GameContextDto &context
      );
      void emitLight(
        LightingSystem &lighting
      ) const;
      void draw(
        Renderer               &renderer,
        const SpriteSheetProps &sprites
      ) const;

    private:
      static constexpr float lifetime_seconds = 10;
      static constexpr float pulse_seconds    = 0.75F;
      static constexpr float noise_radius     = 520;

      [[nodiscard]]
      float blink() const {
        return pulse_age_seconds_ < 0.15F ? 1.0F : 0.2F;
      }

      Vec2Props position_;
      float     remaining_seconds_ = lifetime_seconds;
      float     pulse_timer_       = 0;
      float     pulse_age_seconds_ = 10;
  };
} // namespace wheel_of_fear
