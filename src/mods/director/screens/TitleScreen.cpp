#include "mods/director/screens/TitleScreen.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <string_view>
#include <utility>

#include "core/math/ScalarMath.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/UiPainter.hpp"
#include "core/theme/Palette.hpp"

namespace wheel_of_fear {
  void TitleScreen::draw(
    Renderer &renderer,
    float     real_seconds
  ) {
    constexpr float center_x = RenderConfig::ui_width / 2;

    renderer.drawUiRectangle(
      0,
      0,
      RenderConfig::ui_width,
      RenderConfig::ui_height,
      Palette::panel_deep.withAlpha(0.55F)
    );

    renderer.drawUiRectangle(
      0,
      74,
      RenderConfig::ui_width,
      150,
      ColorProps::black().withAlpha(0.35F)
    );

    const float glitch_intensity =
      ScalarMath::hashTime(real_seconds, 5) > 0.82F ? 1.0F : 0.15F;

    UiPainter::drawGlitchTitle(
      renderer,
      "WHEEL",
      center_x,
      86,
      72,
      Palette::neon_cyan,
      real_seconds,
      glitch_intensity
    );

    const float flicker =
      ScalarMath::hashTime(real_seconds, 11) > 0.93F ? 0.3F : 1.0F;

    renderer.drawDisplayText(
      "OF FEAR",
      center_x,
      166,
      40,
      Palette::hud_magenta.withAlpha(flicker),
      TextAlignEnum::center
    );

    renderer.drawText(
      "SECTOR GRID OFFLINE  ||  RESTORE THE GENERATORS  ||  STAY IN THE LIGHT",
      center_x,
      222,
      15,
      Palette::muted_text,
      TextAlignEnum::center
    );

    constexpr std::array<std::pair<std::string_view, std::string_view>, 9>
      controls {
        {
         {"WASD", "move  -  SHIFT runs, and runs loud"},
         {"MOUSE", "aim the flashlight  -  light pins them in place"},
         {"F", "white beam | ultraviolet | off"},
         {"E", "hold to charge generators and fabricate  -  hide in lockers"},
         {
            "SPACE",
            "hit the sync window while charging  -  hold your breath when hidden",
          }, {"RMB", "throw a lure beacon to pull them away"},
         {"G", "throw a neon grenade  -  its liquid light burns them"},
         {"LMB", "hold to embrace a creature until it turns to plaster"},
         {"TAB", "tech tracker  -  fills your view, kills your light"},
         },
    };

    constexpr float panel_top = 252;

    UiPainter::drawPanel(
      renderer,
      center_x - 300,
      panel_top,
      600,
      232,
      Palette::panel_edge,
      1
    );

    for(std::size_t index = 0; index < controls.size(); ++index) {
      const auto &[key, text] = controls[index];

      const float y = panel_top + 14 + (static_cast<float>(index) * 23.5F);
      const float key_width = renderer.measureText(key, 13) + 11.7F;

      UiPainter::drawKeycap(
        renderer,
        key,
        center_x - 160 - std::max(key_width, 20.8F),
        y,
        13,
        Palette::neon_cyan
      );

      renderer.drawText(
        text,
        center_x - 146,
        y + 1,
        15,
        Palette::text,
        TextAlignEnum::left
      );
    }

    const float pulse = 0.45F + (0.55F * std::sin(real_seconds * 3));

    renderer.drawDisplayText(
      "PRESS ENTER",
      center_x,
      500,
      22,
      Palette::hud_magenta.withAlpha(pulse),
      TextAlignEnum::center
    );

    UiPainter::drawScanlines(
      renderer,
      RenderConfig::ui_width,
      RenderConfig::ui_height,
      0.12F
    );
  }
} // namespace wheel_of_fear
