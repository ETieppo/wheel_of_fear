#include "mods/director/screens/MenuButtonWidget.hpp"
#include "core/render/UiPainter.hpp"
#include "core/text/Localization.hpp"
#include "core/theme/Palette.hpp"
#include "mods/hud/KeyLabelCatalog.hpp"

namespace wheel_of_fear {
  void MenuButtonWidget::draw(
    Renderer              &renderer,
    const MenuButtonProps &button,
    Vec2Props              mouse_in_ui
  ) {
    const RectProps &bounds     = button.bounds;
    const bool       is_hovered = bounds.contains(mouse_in_ui);
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
      KeyLabelCatalog::labelFor(button.action),
      bounds.x + 12,
      bounds.y + 8,
      13,
      accent
    );

    renderer.drawDisplayText(
      Localization::text(button.text),
      bounds.x + 40,
      bounds.y + 7,
      16,
      is_hovered ? Palette::accent : Palette::text,
      TextAlignEnum::left
    );
  }

  bool MenuButtonWidget::wasActivated(
    const MenuButtonProps &button,
    const InputController &input
  ) {
    return input.wasPressed(button.action)
      || (input.wasPressed(InputActionEnum::hug)
          && button.bounds.contains(input.mouseInUi()));
  }
} // namespace wheel_of_fear
