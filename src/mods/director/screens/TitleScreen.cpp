#include "mods/director/screens/TitleScreen.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/UiPainter.hpp"
#include "core/text/Localization.hpp"
#include "core/theme/Palette.hpp"
#include "mods/director/screens/MenuButtonCatalog.hpp"
#include "mods/director/screens/MenuButtonWidget.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <format>
#include <string_view>
#include <utility>

namespace wheel_of_fear {
  namespace {
    constexpr float controls_panel_width = 780;
    constexpr float controls_key_column  = 250;
    constexpr float controls_text_column = 236;

    constexpr LocalizedTextProps tagline {
      "SECTOR GRID OFFLINE  ||  RESTORE THE GENERATORS  ||  STAY IN THE LIGHT",
      "REDE DO SETOR DESLIGADA  ||  RESTAURE OS GERADORES  ||  FIQUE NA LUZ",
    };
    constexpr LocalizedTextProps continue_prompt {
      "PRESS ENTER TO CONTINUE SECTOR {:02}",
      "APERTE ENTER PARA CONTINUAR O SETOR {:02}",
    };
    constexpr LocalizedTextProps new_run_hint {
      "R  -  start a new run from sector 01",
      "R  -  começar uma nova partida do setor 01",
    };
    constexpr LocalizedTextProps start_prompt {
      "PRESS ENTER",
      "APERTE ENTER",
    };

    constexpr std::array<std::pair<std::string_view, LocalizedTextProps>, 9>
      controls {
        {
         {"WASD",
           {"move  -  SHIFT runs, and runs loud",
            "mover  -  SHIFT corre, e corre fazendo barulho"}},
         {"MOUSE",
           {"aim the flashlight  -  light pins them in place",
            "mirar a lanterna  -  a luz os prende no lugar"}},
         {"F",
           {"white beam | ultraviolet | off",
            "feixe branco | ultravioleta | desligada"}},
         {"E",
           {"hold to charge generators and fabricate  -  lockers and letters",
            "segure para carregar geradores e fabricar  -  armários e cartas"}},
         {"SPACE",
           {"hit the sync window while charging  -  hold your breath when hidden",
            "acerte a sincronia ao carregar  -  prenda o fôlego escondido"}},
         {"RMB",
           {"throw a lure beacon to pull them away",
            "jogue um sinalizador para atraí-los para longe"}},
         {"G",
           {"throw a neon grenade  -  its liquid light burns them",
            "jogue uma granada neon  -  a luz líquida os queima"}},
         {"LMB",
           {"hold to embrace a creature until it turns to plaster",
            "segure para abraçar uma criatura até ela virar gesso"}},
         {"TAB",
           {"tech tracker  -  fills your view, kills your light",
            "rastreador  -  ocupa sua visão, apaga sua luz"}},
         },
    };
  } // namespace

  void TitleScreen::draw(
    Renderer                    &renderer,
    float                        real_seconds,
    std::optional<std::uint32_t> saved_phase,
    Vec2Props                    mouse_in_ui
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

    const float glitch_intensity
      = ScalarMath::hashTime(real_seconds, 5) > 0.82F ? 1.0F : 0.15F;

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

    const float flicker
      = ScalarMath::hashTime(real_seconds, 11) > 0.93F ? 0.3F : 1.0F;

    renderer.drawDisplayText(
      "OF FEAR",
      center_x,
      166,
      40,
      Palette::accent.withAlpha(flicker),
      TextAlignEnum::center
    );

    renderer.drawText(
      Localization::text(tagline),
      center_x,
      222,
      15,
      Palette::muted_text,
      TextAlignEnum::center
    );

    constexpr float panel_top = 252;

    UiPainter::drawPanel(
      renderer,
      center_x - (controls_panel_width / 2),
      panel_top,
      controls_panel_width,
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
        center_x - controls_key_column - std::max(key_width, 20.8F),
        y,
        13,
        Palette::neon_cyan
      );

      renderer.drawText(
        Localization::text(text),
        center_x - controls_text_column,
        y + 1,
        15,
        Palette::text,
        TextAlignEnum::left
      );
    }

    const float pulse = 0.45F + (0.55F * std::sin(real_seconds * 3));

    if(saved_phase) {
      renderer.drawDisplayText(
        Localization::format(continue_prompt, *saved_phase),
        center_x,
        500,
        22,
        Palette::accent.withAlpha(pulse),
        TextAlignEnum::center
      );

      renderer.drawText(
        Localization::text(new_run_hint),
        center_x,
        528,
        15,
        Palette::muted_text,
        TextAlignEnum::center
      );
    } else {
      renderer.drawDisplayText(
        Localization::text(start_prompt),
        center_x,
        500,
        22,
        Palette::accent.withAlpha(pulse),
        TextAlignEnum::center
      );
    }

    MenuButtonWidget::draw(
      renderer,
      MenuButtonCatalog::title_credits,
      mouse_in_ui
    );
    MenuButtonWidget::draw(
      renderer,
      MenuButtonCatalog::title_quit,
      mouse_in_ui
    );
    MenuButtonWidget::draw(
      renderer,
      MenuButtonCatalog::title_language,
      mouse_in_ui
    );

    UiPainter::drawScanlines(
      renderer,
      RenderConfig::ui_width,
      RenderConfig::ui_height,
      0.12F
    );
  }
} // namespace wheel_of_fear
