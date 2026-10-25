#include "mods/hud/widgets/MobInspectorWidget.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/UiPainter.hpp"
#include "core/theme/Palette.hpp"
#include <algorithm>

namespace wheel_of_fear {
  void MobInspectorWidget::draw(
    Renderer        &renderer,
    std::string_view mob_name
  ) {
    constexpr float center_x = RenderConfig::ui_width / 2;
    const float     width    = std::max(
      minimum_width,
      renderer.measureText(mob_name, text_size) + (panel_padding * 2)
    );

    UiPainter::drawPanel(
      renderer,
      center_x - (width / 2),
      panel_top,
      width,
      panel_height,
      Palette::panel_edge,
      1
    );

    renderer.drawText(
      mob_name,
      center_x,
      panel_top + 6,
      text_size,
      Palette::text,
      TextAlignEnum::center
    );
  }
} // namespace wheel_of_fear
