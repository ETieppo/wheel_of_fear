#include "mods/hud/widgets/SkillCheckDial.hpp"

#include <cmath>
#include <string_view>

#include "core/math/Scalar.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/UiKit.hpp"
#include "core/theme/Palette.hpp"
#include "mods/generators/SkillCheck.hpp"

namespace wheel_of_fear {
  namespace {
    constexpr Vec2 ring_center {
      RenderConfig::ui_width / 2,
      RenderConfig::ui_height / 2 + 70
    };
    constexpr float ring_radius = 42;
    constexpr int   ring_dots   = 72;
  } // namespace

  void SkillCheckDial::drawResult(
    Renderer         &renderer,
    const SkillCheck &check,
    float             real_seconds
  ) {
    if(check.resultSeconds() <= 0) return;
    const float      alpha = Scalar::clamp(check.resultSeconds() / 0.4F, 0, 1);
    std::string_view label;
    Color            color = Palette::success;
    switch(check.lastResult()) {
      case SkillCheckResultEnum::perfect:
        label = "PERFECT SYNC";
        color = Palette::neon_cyan;
        break;
      case SkillCheckResultEnum::good: label = "SYNCED"; break;
      case SkillCheckResultEnum::failed:
        label = "SHORT CIRCUIT";
        color = Palette::danger;
        break;
      case SkillCheckResultEnum::none: break;
    }
    const float intensity =
      check.lastResult() == SkillCheckResultEnum::failed ? 1.0F : 0.2F;
    UiKit::drawGlitchTitle(
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

  void SkillCheckDial::draw(
    Renderer         &renderer,
    const SkillCheck &check,
    float             real_seconds
  ) {
    if(!check.isActive()) {
      drawResult(renderer, check, real_seconds);
      return;
    }
    const float warning_alpha =
      check.isWarning() ? 0.5F + 0.5F * std::sin(real_seconds * 40) : 1.0F;
    renderer.drawUiShape(
      QuadKindEnum::soft_disc,
      ring_center,
      Vec2::of(ring_radius * 3, ring_radius * 3),
      Palette::panel_deep.withAlpha(0.6F),
      0,
      1.5F
    );
    for(int dot = 0; dot < ring_dots; ++dot) {
      const float angle =
        static_cast<float>(dot) / static_cast<float>(ring_dots) * Scalar::tau;
      const bool in_zone =
        angle >= check.zoneStart() && angle <= check.zoneEnd();
      const bool in_perfect = angle >=
                              check.perfectStart() &&
                              angle <=
                              check.perfectStart() +
                              check.perfectWidth();
      Color      color      = Color::hex(0x2a3448);
      if(in_zone) color = Palette::neon_cyan;
      if(in_perfect) color = Color::white();
      const float size  = in_zone ? 4.0F : 2.5F;
      const Vec2  point = ring_center.add(
        Vec2::fromAngle(angle - Scalar::pi / 2.0F).scale(ring_radius)
      );
      renderer.drawUiShape(
        QuadKindEnum::solid,
        point,
        Vec2::of(size, size),
        color.withAlpha(warning_alpha),
        angle,
        0
      );
    }
    const float needle_angle     = check.needleAngle() - Scalar::pi / 2.0F;
    const Vec2  needle_direction = Vec2::fromAngle(needle_angle);
    renderer.drawUiShape(
      QuadKindEnum::solid,
      ring_center.add(needle_direction.scale(ring_radius * 0.6F)),
      Vec2::of(ring_radius * 0.9F, 3),
      Palette::hud_magenta,
      needle_angle,
      0
    );
    UiKit::drawKeycap(
      renderer,
      "SPACE",
      ring_center.x - 27,
      ring_center.y - 11,
      14,
      Palette::neon_cyan.withAlpha(warning_alpha)
    );
  }
} // namespace wheel_of_fear
