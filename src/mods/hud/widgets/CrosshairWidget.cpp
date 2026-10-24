#include "mods/hud/widgets/CrosshairWidget.hpp"
#include "core/math/ScalarMath.hpp"

namespace wheel_of_fear {
  void CrosshairWidget::draw(
    Renderer  &renderer,
    Vec2Props  mouse,
    ColorProps color,
    float      real_seconds
  ) {
    const float spin = real_seconds * 0.8F;

    renderer.drawUiShape(
      QuadKindEnum::ring,
      mouse,
      Vec2Props::of(16, 16),
      color.withAlpha(0.55F),
      0,
      0.14F
    );

    for(int tick = 0; tick < 4; ++tick) {
      const float angle
        = spin + (static_cast<float>(tick) * ScalarMath::pi / 2.0F);

      renderer.drawUiShape(
        QuadKindEnum::solid,
        mouse.add(Vec2Props::fromAngle(angle).scale(12)),
        Vec2Props::of(5, 1.5F),
        color.withAlpha(0.85F),
        angle,
        0
      );
    }

    renderer.drawUiRectangle(mouse.x - 1, mouse.y - 1, 2, 2, color);
  }
} // namespace wheel_of_fear
