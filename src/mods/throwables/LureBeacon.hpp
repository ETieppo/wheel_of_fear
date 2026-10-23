#pragma once

#include "core/math/Vec2.hpp"
#include "core/render/Renderer.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/art/SpriteSheetProps.hpp"
#include "mods/lighting/Lighting.hpp"

namespace wheel_of_fear {

class LureBeacon {
public:
    explicit LureBeacon(Vec2 position) : position_(position) {}

    Vec2 position() const { return position_; }
    bool isExpired() const { return remaining_seconds_ <= 0; }
    void update(GameContextDto& context);
    void emitLight(Lighting& lighting) const;
    void draw(Renderer& renderer, const SpriteSheetProps& sprites) const;

private:
    static constexpr float lifetime_seconds = 10;
    static constexpr float pulse_seconds = 0.75F;
    static constexpr float noise_radius = 520;

    float blink() const { return pulse_age_seconds_ < 0.15F ? 1.0F : 0.2F; }

    Vec2 position_;
    float remaining_seconds_ = lifetime_seconds;
    float pulse_timer_ = 0;
    float pulse_age_seconds_ = 10;
};

}
