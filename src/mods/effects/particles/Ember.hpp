#pragma once

#include "mods/effects/particles/Particle.hpp"

namespace wheel_of_fear {

class Ember final : public Particle {
public:
    using Particle::Particle;

    void draw(Renderer& renderer, const SpriteSheetProps& sprites) const override {
        renderer.drawGlowSprite(sprites.light[0], state().position, state().color.withAlpha(life() * 0.9F), SpriteDrawDto{.scale = 0.12F + 0.3F * life()});
    }

protected:
    float drag() const override { return 2.5F; }
};

}
