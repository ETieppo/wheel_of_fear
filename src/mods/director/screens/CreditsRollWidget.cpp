#include "mods/director/screens/CreditsRollWidget.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/theme/Palette.hpp"

namespace wheel_of_fear {
  void CreditsRollWidget::draw(
    Renderer                        &renderer,
    std::span<const CreditLineProps> lines,
    float                            scroll,
    float                            backdrop_alpha
  ) {
    constexpr float center_x = RenderConfig::ui_width / 2;

    renderer.drawUiRectangle(
      0,
      0,
      RenderConfig::ui_width,
      RenderConfig::ui_height,
      Palette::panel_deep.withAlpha(backdrop_alpha)
    );

    float y = RenderConfig::ui_height - scroll;

    for(const CreditLineProps &line : lines) {
      const bool is_on_screen
        = y + line.size >= 0 && y <= RenderConfig::ui_height;

      if(is_on_screen && line.is_display)
        renderer.drawDisplayText(
          line.text,
          center_x,
          y,
          line.size,
          line.color,
          TextAlignEnum::center
        );

      if(is_on_screen && !line.is_display)
        renderer.drawText(
          line.text,
          center_x,
          y,
          line.size,
          line.color,
          TextAlignEnum::center
        );

      y += line.size + line.spacing_after;
    }
  }
} // namespace wheel_of_fear
