#pragma once

#include "mods/effects/particles/Particle.hpp"

namespace wheel_of_fear {
  class AcidDropParticle final: public Particle {
    public:
      using Particle::Particle;

      void draw(
        Renderer               &renderer,
        const SpriteSheetProps &sprites
      ) const override {
        renderer.drawGlowSprite(
          sprites.light[1],
          state().position,
          state().color.withAlpha(life()),
          SpriteDrawDto {.scale = 0.12F}
        );
      }

    protected:
      [[nodiscard]]
      float drag() const override {
        return 1;
      }

      [[nodiscard]]
      bool fallsWithGravity() const override {
        return true;
      }
  };
} // namespace wheel_of_fear
