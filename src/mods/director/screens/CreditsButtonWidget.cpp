#include "mods/director/screens/CreditsButtonWidget.hpp"
#include "core/render/UiPainter.hpp"
#include "core/theme/Palette.hpp"

namespace wheel_of_fear {
  void CreditsButtonWidget::draw(
    Renderer &renderer,
    bool      is_hovered
  ) {
    const ColorProps accent
      = is_hovered ? Palette::accent : Palette::panel_edge;

    UiPainter::drawPanel(
      renderer,
      bounds.x,
      bounds.y,
      bounds.width,
      bounds.height,
      accent,
      is_hovered ? 1.0F : 0.7F
    );

    UiPainter::drawKeycap(
      renderer,
      "C",
      bounds.x + 12,
      bounds.y + 8,
      13,
      accent
    );

    renderer.drawDisplayText(
      "CREDITS",
      bounds.x + 40,
      bounds.y + 7,
      16,
      is_hovered ? Palette::accent : Palette::text,
      TextAlignEnum::left
    );
  }
} // namespace wheel_of_fear
