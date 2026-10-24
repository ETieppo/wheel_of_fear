#include "mods/director/screens/PhaseIntroScreen.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/UiPainter.hpp"
#include "core/theme/Palette.hpp"
#include <format>

namespace wheel_of_fear {
  void PhaseIntroScreen::draw(
    Renderer                         &renderer,
    std::uint32_t                     phase,
    std::span<const std::string_view> introductions,
    float                             state_seconds,
    float                             intro_seconds,
    float                             real_seconds
  ) {
    constexpr float center_x = RenderConfig::ui_width / 2;
    const float     appear   = ScalarMath::clamp(state_seconds * 2, 0, 1);
    const float     vanish
      = ScalarMath::clamp((intro_seconds - state_seconds) * 2, 0, 1);
    const float alpha  = appear * vanish;
    const float height = 150 + (static_cast<float>(introductions.size()) * 24);

    renderer.drawUiRectangle(
      0,
      140,
      RenderConfig::ui_width,
      height,
      Palette::panel_deep.withAlpha(0.82F * alpha)
    );

    renderer.drawUiRectangle(
      0,
      140,
      RenderConfig::ui_width,
      1,
      Palette::panel_edge.withAlpha(alpha)
    );

    renderer.drawUiRectangle(
      0,
      140 + height,
      RenderConfig::ui_width,
      1,
      Palette::hud_magenta.withAlpha(alpha)
    );

    UiPainter::drawGlitchTitle(
      renderer,
      std::format("SECTOR {:02}", phase),
      center_x,
      156,
      46,
      Palette::neon_cyan.withAlpha(alpha),
      real_seconds,
      state_seconds < 0.6F ? 1.0F : 0.1F
    );

    renderer.drawText(
      "Charge every generator. Their rooms light up and burn the "
      "dark. Then reach the exit.",
      center_x,
      222,
      16,
      Palette::text.withAlpha(alpha),
      TextAlignEnum::center
    );

    renderer.drawText(
      "They freeze in your beam. They hunt by sound. The rooms "
      "around you never stay the same.",
      center_x,
      244,
      16,
      Palette::muted_text.withAlpha(alpha),
      TextAlignEnum::center
    );

    for(std::size_t index = 0; index < introductions.size(); ++index) {
      renderer.drawText(
        introductions[index],
        center_x,
        280 + (static_cast<float>(index) * 24),
        15,
        Palette::hud_magenta.withAlpha(alpha),
        TextAlignEnum::center
      );
    }

    renderer.drawText(
      "ENTER  SKIP",
      center_x,
      140 + height - 22,
      12,
      Palette::muted_text.withAlpha(alpha * 0.8F),
      TextAlignEnum::center
    );
  }
} // namespace wheel_of_fear
