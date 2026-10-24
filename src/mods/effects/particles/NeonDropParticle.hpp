#pragma once

#include "mods/effects/particles/Particle.hpp"

namespace wheel_of_fear {
  class NeonDropParticle final: public Particle {
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
          SpriteDrawDto {.scale = 0.08F + (0.1F * life())}
        );
      }

    protected:
      [[nodiscard]]
      float drag() const override {
        return 2.2F;
      }

      [[nodiscard]]
      bool fallsWithGravity() const override {
        return true;
      }
  };
} // namespace wheel_of_fear
