#pragma once

#include "mods/effects/particles/Particle.hpp"

namespace wheel_of_fear {

class NeonDrop final : public Particle {
public:
    using Particle::Particle;

    void draw(Renderer& renderer, const SpriteSheetProps& sprites) const override {
        renderer.drawGlowSprite(sprites.light[1], state().position, state().color.withAlpha(life()), SpriteDrawDto{.scale = 0.08F + 0.1F * life()});
    }

protected:
    float drag() const override { return 2.2F; }
    bool fallsWithGravity() const override { return true; }
};

}
