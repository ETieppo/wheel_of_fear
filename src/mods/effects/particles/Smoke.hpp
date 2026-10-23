#pragma once

#include "mods/effects/particles/Particle.hpp"

namespace wheel_of_fear {

class Smoke final : public Particle {
public:
    using Particle::Particle;

    void draw(Renderer& renderer, const SpriteSheetProps& sprites) const override {
        const SpriteDrawDto options{.rotation = state().rotation, .color = state().color.withAlpha(0.4F * life()), .scale = state().size * (1.5F - life() * 0.5F) / 7};
        renderer.drawSprite(sprites.smoke[state().variant % sprites.smoke.size()], state().position, options);
    }

protected:
    float drag() const override { return 1.2F; }
};

}
