#include "mods/effects/EffectSystem.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/ArtLibrary.hpp"
#include "mods/effects/particles/AcidDropParticle.hpp"
#include "mods/effects/particles/EmberParticle.hpp"
#include "mods/effects/particles/NeonDropParticle.hpp"
#include "mods/effects/particles/PlasterChunkParticle.hpp"
#include "mods/effects/particles/SmokeParticle.hpp"
#include "mods/effects/particles/SparkParticle.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/world/WorldSystem.hpp"
#include <algorithm>

namespace wheel_of_fear {
  void EffectSystem::clear() {
    particles_.clear();
    flash_strength_ = 0;
  }

  void EffectSystem::emitSparks(
    WorldSystem &world,
    Vec2Props    origin,
    std::size_t  amount,
    ColorProps   color
  ) {
    RandomGenerator &random = world.random();

    for(std::size_t index = 0; index < amount; ++index) {
      ParticleProps state;
      state.lifetime_seconds  = random.range(0.2F, 0.6F);
      state.remaining_seconds = state.lifetime_seconds;
      state.position          = origin;
      state.velocity          = random.direction().scale(random.range(30, 110));
      state.color             = color;
      state.variant           = random.index(7);
      spawn<SparkParticle>(state);
    }
  }

  void EffectSystem::emitCrumble(
    WorldSystem &world,
    Vec2Props    origin,
    std::size_t  amount
  ) {
    RandomGenerator &random = world.random();

    for(std::size_t index = 0; index < amount; ++index) {
      ParticleProps state;
      state.lifetime_seconds  = random.range(0.8F, 1.8F);
      state.remaining_seconds = state.lifetime_seconds;
      state.position = origin.add(random.direction().scale(random.range(0, 6)));
      state.velocity = random.direction()
                         .scale(random.range(10, 55))
                         .add(Vec2Props::of(0, -20));
      state.spin     = random.range(-8, 8);
      state.variant  = random.index(3);
      spawn<PlasterChunkParticle>(state);
    }

    for(std::size_t index = 0; index < amount / 2; ++index) {
      ParticleProps state;
      state.lifetime_seconds  = random.range(0.8F, 1.6F);
      state.remaining_seconds = state.lifetime_seconds;
      state.position = origin.add(random.direction().scale(random.range(0, 5)));
      state.velocity = random.direction()
                         .scale(random.range(4, 14))
                         .add(Vec2Props::of(0, -8));
      state.size     = random.range(3, 7);
      state.color    = ColorProps::hex(0xb8b2a6);
      state.spin     = random.range(-1, 1);
      state.variant  = random.index(5);
      spawn<SmokeParticle>(state);
    }
  }

  void EffectSystem::emitExplosion(
    WorldSystem &world,
    Vec2Props    origin
  ) {
    RandomGenerator &random = world.random();
    flash_position_         = origin;
    flash_strength_         = 1;

    for(int index = 0; index < 40; ++index) {
      ParticleProps state;
      state.lifetime_seconds  = random.range(0.3F, 0.9F);
      state.remaining_seconds = state.lifetime_seconds;
      state.position          = origin;
      state.velocity          = random.direction().scale(random.range(40, 170));
      state.color             = ColorProps::hex(0xffa040);
      spawn<EmberParticle>(state);
    }

    for(int index = 0; index < 18; ++index) {
      ParticleProps state;
      state.lifetime_seconds  = random.range(1.0F, 2.2F);
      state.remaining_seconds = state.lifetime_seconds;
      state.position
        = origin.add(random.direction().scale(random.range(0, 10)));

      state.velocity = random.direction().scale(random.range(8, 30));
      state.size     = random.range(6, 12);
      state.color    = ColorProps::hex(0x3a3532);
      state.spin     = random.range(-1, 1);
      state.variant  = random.index(5);
      spawn<SmokeParticle>(state);
    }

    world.addScorchMark(origin);
  }

  void EffectSystem::emitNeonBurst(
    WorldSystem &world,
    Vec2Props    origin
  ) {
    RandomGenerator &random = world.random();

    for(int index = 0; index < 46; ++index) {
      ParticleProps state;
      state.lifetime_seconds  = random.range(0.35F, 1.0F);
      state.remaining_seconds = state.lifetime_seconds;
      state.position          = origin;
      state.velocity          = random.direction()
                                  .scale(random.range(30, 150))
                                  .add(Vec2Props::of(0, -20));
      state.color
        = random.chance(0.7F) ? Palette::neon_magenta : Palette::neon_cyan;

      spawn<NeonDropParticle>(state);
    }

    emitSparks(world, origin, 18, Palette::neon_magenta);
  }

  void EffectSystem::emitAcidDrip(
    WorldSystem &world,
    Vec2Props    origin
  ) {
    RandomGenerator &random = world.random();
    ParticleProps    state;

    state.lifetime_seconds  = random.range(0.4F, 0.8F);
    state.remaining_seconds = state.lifetime_seconds;
    state.position          = origin;
    state.velocity
      = Vec2Props::of(random.range(-10, 10), random.range(-30, -10));

    state.color = ColorProps::hex(0x9be860);
    spawn<AcidDropParticle>(state);
  }

  void EffectSystem::update(GameContextDto &context) {
    const float delta = context.clock.realDeltaSeconds();
    flash_strength_   = std::max(0.0F, flash_strength_ - (delta * 3));
    std::size_t index = 0;

    if(flash_strength_ > 0) {
      LightProps flash;
      flash.position  = flash_position_;
      flash.radius    = 140;
      flash.intensity = 2.5F * flash_strength_;
      flash.color     = ColorProps::hex(0xffb060);
      flash.flicker   = 0.3F;
      context.lighting.add(flash);
    }

    while(index < particles_.size()) {
      if(particles_[index]->advance(delta)) {
        index += 1;
        continue;
      }

      particles_[index] = std::move(particles_.back());
      particles_.pop_back();
    }
  }

  void EffectSystem::draw(GameContextDto &context) const {
    const SpriteSheetProps &sprites = context.art.sprites();

    for(const auto &particle : particles_)
      if(context.renderer.isVisible(particle->position(), 16))
        particle->draw(context.renderer, sprites);
  }
} // namespace wheel_of_fear
