#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "core/math/Color.hpp"
#include "core/math/Vec2.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/effects/particles/Particle.hpp"

namespace wheel_of_fear {

class Effects {
public:
    Effects() { particles_.reserve(capacity); }

    void clear();
    void emitSparks(World& world, Vec2 origin, std::size_t amount, Color color);
    void emitCrumble(World& world, Vec2 origin, std::size_t amount);
    void emitExplosion(World& world, Vec2 origin);
    void emitNeonBurst(World& world, Vec2 origin);
    void emitAcidDrip(World& world, Vec2 origin);
    void update(GameContextDto& context);
    void draw(GameContextDto& context) const;

private:
    static constexpr std::size_t capacity = 1024;

    template <typename ParticleType>
    void spawn(const ParticleProps& state) {
        if (particles_.size() >= capacity) return;
        particles_.push_back(std::make_unique<ParticleType>(state));
    }

    std::vector<std::unique_ptr<Particle>> particles_;
    Vec2 flash_position_{};
    float flash_strength_ = 0;
};

}
