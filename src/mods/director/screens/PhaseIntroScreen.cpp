#include "mods/director/screens/PhaseIntroScreen.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/UiPainter.hpp"
#include "core/text/Localization.hpp"
#include "core/theme/Palette.hpp"
#include <format>

namespace wheel_of_fear {
  namespace {
    constexpr LocalizedTextProps sector_title {
      "SECTOR {:02}",
      "SETOR {:02}",
    };
    constexpr LocalizedTextProps goal_text {
      "Charge every generator. Their rooms light up and burn the dark. Then find the letter with the key and reach the exit.",
      "Carregue todos os geradores. As salas acendem e queimam o escuro. Depois ache a carta com a chave e chegue à saída.",
    };
    constexpr LocalizedTextProps skip_hint {
      "ENTER  SKIP",
      "ENTER  PULAR",
    };
  } // namespace

  void PhaseIntroScreen::draw(
    Renderer     &renderer,
    std::uint32_t phase,
    float         state_seconds,
    float         intro_seconds,
    float         real_seconds
  ) {
    constexpr float center_x = RenderConfig::ui_width / 2;
    const float     appear   = ScalarMath::clamp(state_seconds * 2, 0, 1);
    const float     vanish
      = ScalarMath::clamp((intro_seconds - state_seconds) * 2, 0, 1);
    const float     alpha  = appear * vanish;
    constexpr float height = 150;

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
      Palette::accent.withAlpha(alpha)
    );

    UiPainter::drawGlitchTitle(
      renderer,
      Localization::format(sector_title, phase),
      center_x,
      156,
      46,
      Palette::neon_cyan.withAlpha(alpha),
      real_seconds,
      state_seconds < 0.6F ? 1.0F : 0.1F
    );

    renderer.drawText(
      Localization::text(goal_text),
      center_x,
      230,
      16,
      Palette::text.withAlpha(alpha),
      TextAlignEnum::center
    );

    renderer.drawText(
      Localization::text(skip_hint),
      center_x,
      140 + height - 22,
      12,
      Palette::muted_text.withAlpha(alpha * 0.8F),
      TextAlignEnum::center
    );
  }
} // namespace wheel_of_fear
