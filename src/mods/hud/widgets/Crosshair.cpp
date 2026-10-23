#include "mods/hud/widgets/Crosshair.hpp"

#include "core/math/Scalar.hpp"

namespace wheel_of_fear {

void Crosshair::draw(Renderer& renderer, Vec2 mouse, Color color, float real_seconds) {
    const float spin = real_seconds * 0.8F;
    renderer.drawUiShape(QuadKindEnum::ring, mouse, Vec2::of(16, 16), color.withAlpha(0.55F), 0, 0.14F);
    for (int tick = 0; tick < 4; ++tick) {
        const float angle = spin + static_cast<float>(tick) * Scalar::pi / 2.0F;
        renderer.drawUiShape(QuadKindEnum::solid, mouse.add(Vec2::fromAngle(angle).scale(12)), Vec2::of(5, 1.5F), color.withAlpha(0.85F), angle, 0);
    }
    renderer.drawUiRectangle(mouse.x - 1, mouse.y - 1, 2, 2, color);
}

}
