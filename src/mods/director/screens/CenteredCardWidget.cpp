#include "mods/director/screens/CenteredCardWidget.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/UiPainter.hpp"
#include "core/theme/Palette.hpp"

namespace wheel_of_fear {
  void CenteredCardWidget::draw(
    Renderer        &renderer,
    std::string_view title,
    std::string_view subtitle,
    ColorProps       color,
    float            real_seconds
  ) {
    constexpr float center_x = RenderConfig::ui_width / 2;

    UiPainter::drawPanel(renderer, center_x - 280, 190, 560, 130, color, 1);

    renderer.drawUiRectangle(
      0,
      0,
      RenderConfig::ui_width,
      RenderConfig::ui_height,
      ColorProps::black().withAlpha(0.35F)
    );

    UiPainter::drawGlitchTitle(
      renderer,
      title,
      center_x,
      210,
      40,
      color,
      real_seconds,
      0.25F
    );

    renderer.drawText(
      subtitle,
      center_x,
      276,
      15,
      Palette::text,
      TextAlignEnum::center
    );

    UiPainter::drawScanlines(
      renderer,
      RenderConfig::ui_width,
      RenderConfig::ui_height,
      0.08F
    );
  }
} // namespace wheel_of_fear
