#pragma once

#include "core/math/Vec2Props.hpp"
#include "core/render/Renderer.hpp"
#include "mods/art/SpriteSheetProps.hpp"
#include "mods/lighting/LightingSystem.hpp"

namespace wheel_of_fear {
  class ExtraLifePickupEntity {
    public:
      static constexpr float reach = 14;

      explicit ExtraLifePickupEntity(Vec2Props position)
          : position_(position) {
      }

      [[nodiscard]]
      Vec2Props position() const {
        return position_;
      }

      void settleOn(Vec2Props floor) {
        position_ = floor;
      }

      void emitLight(
        LightingSystem &lighting,
        float           real_seconds
      ) const;

      void draw(
        Renderer               &renderer,
        const SpriteSheetProps &sprites,
        float                   real_seconds
      ) const;

    private:
      [[nodiscard]]
      static float pulse(float real_seconds);

      Vec2Props position_;
  };
} // namespace wheel_of_fear
