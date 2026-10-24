#include "mods/hud/widgets/HidingOverlayWidget.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/UiPainter.hpp"
#include "core/theme/Palette.hpp"
#include "mods/player/components/LockerHidingComponent.hpp"
#include <cmath>

namespace wheel_of_fear {
  void HidingOverlayWidget::draw(
    Renderer                    &renderer,
    const LockerHidingComponent &hiding,
    float                        real_seconds
  ) {
    constexpr float width  = RenderConfig::ui_width;
    constexpr float height = RenderConfig::ui_height;

    renderer.drawUiRectangle(
      0,
      0,
      width,
      height * 0.36F,
      ColorProps::black().withAlpha(0.82F)
    );

    renderer.drawUiRectangle(
      0,
      height * 0.64F,
      width,
      height * 0.36F,
      ColorProps::black().withAlpha(0.82F)
    );

    for(int slit = 0; slit < 5; ++slit)
      renderer.drawUiRectangle(
        0,
        (height * 0.36F) + 18 + (static_cast<float>(slit) * 30),
        width,
        12,
        ColorProps::black().withAlpha(0.7F)
      );

    constexpr float center_x = width / 2;

    UiPainter::drawPanel(
      renderer,
      center_x - 130,
      height - 150,
      260,
      44,
      Palette::panel_edge,
      1
    );

    renderer.drawText(
      "BREATH",
      center_x - 116,
      height - 144,
      12,
      Palette::muted_text,
      TextAlignEnum::left
    );

    const ColorProps breath_color
      = hiding.isHoldingBreath() ? Palette::neon_cyan : Palette::muted_text;

    UiPainter::drawSegmentedBar(
      renderer,
      SegmentedBarProps {
        .x             = center_x - 116,
        .y             = height - 126,
        .segment_count = 16,
        .segment_width = 12,
        .height        = 7,
        .fill          = hiding.breath(),
        .color         = breath_color,
        .empty_color   = ColorProps::hex(0x141a26),
      }
    );

    const float pulse = 0.6F + (0.4F * std::sin(real_seconds * 2));

    renderer.drawText(
      "HOLD SPACE  -  HOLD YOUR BREATH",
      center_x + 116,
      height - 144,
      12,
      Palette::text.withAlpha(pulse),
      TextAlignEnum::right
    );
  }
} // namespace wheel_of_fear
