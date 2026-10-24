#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "core/math/ColorProps.hpp"
#include "core/math/Vec2Props.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/effects/particles/Particle.hpp"

namespace wheel_of_fear {
  class EffectSystem {
    public:
      EffectSystem() {
        particles_.reserve(capacity);
      }

      void clear();
      void emitSparks(
        WorldSystem &world,
        Vec2Props    origin,
        std::size_t  amount,
        ColorProps   color
      );
      void emitCrumble(
        WorldSystem &world,
        Vec2Props    origin,
        std::size_t  amount
      );
      void emitExplosion(
        WorldSystem &world,
        Vec2Props    origin
      );
      void emitNeonBurst(
        WorldSystem &world,
        Vec2Props    origin
      );
      void emitAcidDrip(
        WorldSystem &world,
        Vec2Props    origin
      );
      void update(
        GameContextDto &context
      );
      void draw(
        GameContextDto &context
      ) const;

    private:
      static constexpr std::size_t capacity = 1024;

      template<typename ParticleType> void spawn(
        const ParticleProps &state
      ) {
        if(particles_.size() >= capacity) return;

        particles_.push_back(std::make_unique<ParticleType>(state));
      }

      std::vector<std::unique_ptr<Particle>> particles_;
      Vec2Props                              flash_position_ {};
      float                                  flash_strength_ = 0;
  };
} // namespace wheel_of_fear
