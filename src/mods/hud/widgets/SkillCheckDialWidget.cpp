#include "mods/hud/widgets/SkillCheckDialWidget.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/UiPainter.hpp"
#include "core/text/Localization.hpp"
#include "core/theme/Palette.hpp"
#include "mods/generators/SkillCheckMinigame.hpp"
#include <cmath>
#include <string_view>

namespace wheel_of_fear {
  namespace {
    constexpr LocalizedTextProps perfect_label {
      "PERFECT SYNC",
      "SINCRONIA PERFEITA",
    };
    constexpr LocalizedTextProps synced_label {
      "SYNCED",
      "SINCRONIZADO",
    };
    constexpr LocalizedTextProps short_circuit_label {
      "SHORT CIRCUIT",
      "CURTO-CIRCUITO",
    };
    constexpr Vec2Props ring_center {
      .x = RenderConfig::ui_width / 2,
      .y = (RenderConfig::ui_height / 2) + 70,
    };
    constexpr float ring_radius = 42;
    constexpr int   ring_dots   = 72;
  } // namespace

  void SkillCheckDialWidget::drawResult(
    Renderer                 &renderer,
    const SkillCheckMinigame &check,
    float                     real_seconds
  ) {
    if(check.resultSeconds() <= 0) return;
    const float alpha = ScalarMath::clamp(check.resultSeconds() / 0.4F, 0, 1);

    std::string_view label;
    ColorProps       color = Palette::success;

    switch(check.lastResult()) {
      case SkillCheckResultEnum::perfect:
        label = Localization::text(perfect_label);
        color = Palette::neon_cyan;
        break;

      case SkillCheckResultEnum::good:
        label = Localization::text(synced_label);
        break;

      case SkillCheckResultEnum::failed:
        label = Localization::text(short_circuit_label);
        color = Palette::danger;
        break;

      case SkillCheckResultEnum::none: break;
    }

    const float intensity
      = check.lastResult() == SkillCheckResultEnum::failed ? 1.0F : 0.2F;

    UiPainter::drawGlitchTitle(
      renderer,
      label,
      ring_center.x,
      ring_center.y - 14,
      22,
      color.withAlpha(alpha),
      real_seconds,
      intensity
    );
  }

  void SkillCheckDialWidget::draw(
    Renderer                 &renderer,
    const SkillCheckMinigame &check,
    float                     real_seconds
  ) {
    if(!check.isActive()) {
      drawResult(renderer, check, real_seconds);
      return;
    }

    const float warning_alpha
      = check.isWarning() ? 0.5F + (0.5F * std::sin(real_seconds * 40)) : 1.0F;

    renderer.drawUiShape(
      QuadKindEnum::soft_disc,
      ring_center,
      Vec2Props::of(ring_radius * 3, ring_radius * 3),
      Palette::panel_deep.withAlpha(0.6F),
      0,
      1.5F
    );

    for(int dot = 0; dot < ring_dots; ++dot) {
      const float angle = static_cast<float>(dot)
        / static_cast<float>(ring_dots)
        * ScalarMath::tau;
      const bool in_zone
        = angle >= check.zoneStart() && angle <= check.zoneEnd();
      const bool in_perfect = angle
        >= check.perfectStart()
        && angle
        <= check.perfectStart()
        + check.perfectWidth();

      ColorProps color = ColorProps::hex(0x2a3448);
      if(in_zone) color = Palette::neon_cyan;
      if(in_perfect) color = ColorProps::white();

      const float     size  = in_zone ? 4.0F : 2.5F;
      const Vec2Props point = ring_center.add(
        Vec2Props::fromAngle(angle - (ScalarMath::pi / 2.0F)).scale(ring_radius)
      );

      renderer.drawUiShape(
        QuadKindEnum::solid,
        point,
        Vec2Props::of(size, size),
        color.withAlpha(warning_alpha),
        angle,
        0
      );
    }

    const float needle_angle = check.needleAngle() - (ScalarMath::pi / 2.0F);
    const Vec2Props needle_direction = Vec2Props::fromAngle(needle_angle);

    renderer.drawUiShape(
      QuadKindEnum::solid,
      ring_center.add(needle_direction.scale(ring_radius * 0.6F)),
      Vec2Props::of(ring_radius * 0.9F, 3),
      Palette::accent,
      needle_angle,
      0
    );

    UiPainter::drawKeycap(
      renderer,
      "SPACE",
      ring_center.x - 27,
      ring_center.y - 11,
      14,
      Palette::neon_cyan.withAlpha(warning_alpha)
    );
  }
} // namespace wheel_of_fear
