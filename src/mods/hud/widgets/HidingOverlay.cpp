#include "mods/hud/widgets/HidingOverlay.hpp"

#include <cmath>

#include "core/render/RenderConfig.hpp"
#include "core/render/UiKit.hpp"
#include "core/theme/Palette.hpp"
#include "mods/player/LockerHiding.hpp"

namespace wheel_of_fear {
  void HidingOverlay::draw(
    Renderer           &renderer,
    const LockerHiding &hiding,
    float               real_seconds
  ) {
    constexpr float width  = RenderConfig::ui_width;
    constexpr float height = RenderConfig::ui_height;
    renderer.drawUiRectangle(
      0,
      0,
      width,
      height * 0.36F,
      Color::black().withAlpha(0.82F)
    );
    renderer.drawUiRectangle(
      0,
      height * 0.64F,
      width,
      height * 0.36F,
      Color::black().withAlpha(0.82F)
    );
    for(int slit = 0; slit < 5; ++slit)
      renderer.drawUiRectangle(
        0,
        height * 0.36F + 18 + static_cast<float>(slit) * 30,
        width,
        12,
        Color::black().withAlpha(0.7F)
      );

    constexpr float center_x = width / 2;
    UiKit::drawPanel(
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
    const Color breath_color =
      hiding.isHoldingBreath() ? Palette::neon_cyan : Palette::muted_text;
    UiKit::drawSegmentedBar(
      renderer,
      center_x - 116,
      height - 126,
      16,
      12,
      7,
      hiding.breath(),
      breath_color,
      Color::hex(0x141a26)
    );
    const float pulse = 0.6F + 0.4F * std::sin(real_seconds * 2);
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
