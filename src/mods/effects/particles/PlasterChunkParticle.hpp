#pragma once

#include "core/math/ScalarMath.hpp"
#include "mods/effects/particles/Particle.hpp"

namespace wheel_of_fear {
  class PlasterChunkParticle final: public Particle {
    public:
      using Particle::Particle;

      void draw(
        Renderer               &renderer,
        const SpriteSheetProps &sprites
      ) const override {
        const SpriteDrawDto options {
          .rotation = state().rotation,
          .color    = ColorProps::hex(0xd6d0c4).withAlpha(
            ScalarMath::clamp(life() * 3, 0, 1)
          ),
          .scale = 0.2F,
        };

        renderer.drawSprite(
          sprites.dirt[state().variant % sprites.dirt.size()],
          state().position,
          options
        );
      }

    protected:
      [[nodiscard]]
      float drag() const override {
        return 3;
      }

      [[nodiscard]]
      bool fallsWithGravity() const override {
        return true;
      }
  };
} // namespace wheel_of_fear
