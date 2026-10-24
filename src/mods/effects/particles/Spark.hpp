#pragma once

#include "mods/effects/particles/Particle.hpp"

namespace wheel_of_fear {
  class Spark final: public Particle {
    public:
      using Particle::Particle;

      void draw(
        Renderer               &renderer,
        const SpriteSheetProps &sprites
      ) const override {
        const SpriteDrawDto options {
          .rotation = state().velocity.angle(),
          .scale    = 0.18F + 0.12F * life()
        };
        renderer.drawGlowSprite(
          sprites.spark[state().variant % sprites.spark.size()],
          state().position,
          state().color.withAlpha(life()),
          options
        );
      }

    protected:
      float drag() const override {
        return 2.5F;
      }
  };
} // namespace wheel_of_fear
