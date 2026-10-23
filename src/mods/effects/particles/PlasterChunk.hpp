#pragma once

#include "core/math/Scalar.hpp"
#include "mods/effects/particles/Particle.hpp"

namespace wheel_of_fear {

class PlasterChunk final : public Particle {
public:
    using Particle::Particle;

    void draw(Renderer& renderer, const SpriteSheetProps& sprites) const override {
        const SpriteDrawDto options{.rotation = state().rotation, .color = Color::hex(0xd6d0c4).withAlpha(Scalar::clamp(life() * 3, 0, 1)), .scale = 0.2F};
        renderer.drawSprite(sprites.dirt[state().variant % sprites.dirt.size()], state().position, options);
    }

protected:
    float drag() const override { return 3; }
    bool fallsWithGravity() const override { return true; }
};

}
