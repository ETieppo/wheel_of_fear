#pragma once

#include "core/render/Renderer.hpp"
#include "mods/art/SpriteSheetProps.hpp"
#include "mods/effects/ParticleProps.hpp"

namespace wheel_of_fear {

class Particle {
public:
    explicit Particle(const ParticleProps& state) : state_(state) {}
    virtual ~Particle() = default;

    Vec2 position() const { return state_.position; }
    bool advance(float delta_seconds);
    virtual void draw(Renderer& renderer, const SpriteSheetProps& sprites) const = 0;

protected:
    virtual float drag() const = 0;
    virtual bool fallsWithGravity() const { return false; }

    const ParticleProps& state() const { return state_; }
    float life() const { return state_.remaining_seconds / state_.lifetime_seconds; }

private:
    static constexpr float gravity = 60;

    ParticleProps state_;
};

}
