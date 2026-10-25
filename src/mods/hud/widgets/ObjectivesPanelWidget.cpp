#include "mods/hud/widgets/ObjectivesPanelWidget.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/UiPainter.hpp"
#include "core/text/Localization.hpp"
#include "core/theme/Palette.hpp"
#include "mods/generators/GeneratorSystem.hpp"
#include <format>
#include <string>

namespace wheel_of_fear {
  namespace {
    constexpr LocalizedTextProps sector_label {
      "SECTOR {:02}",
      "SETOR {:02}",
    };
    constexpr LocalizedTextProps exit_open_label {
      "EXIT OPEN",
      "SAÍDA ABERTA",
    };
    constexpr LocalizedTextProps exit_sealed_label {
      "EXIT SEALED",
      "SAÍDA LACRADA",
    };
    constexpr LocalizedTextProps key_found_label {
      "KEY FOUND",
      "CHAVE ENCONTRADA",
    };
    constexpr LocalizedTextProps key_missing_label {
      "THE KEY IS IN A LETTER",
      "A CHAVE ESTÁ EM UMA CARTA",
    };
  } // namespace

  void ObjectivesPanelWidget::draw(
    Renderer              &renderer,
    std::uint32_t          phase,
    const GeneratorSystem &generators,
    bool                   has_key
  ) {
    constexpr float width = 220;
    constexpr float left  = RenderConfig::ui_width - 14 - width;

    UiPainter::drawPanel(renderer, left, 14, width, 88, Palette::accent, 1);

    renderer.drawText(
      Localization::text(has_key ? key_found_label : key_missing_label),
      left + 16,
      76,
      12,
      has_key ? Palette::accent : Palette::muted_text,
      TextAlignEnum::left
    );

    renderer.drawText(
      Localization::format(sector_label, phase),
      left + 16,
      19,
      15,
      Palette::accent,
      TextAlignEnum::left
    );

    const bool is_unlocked = generators.exitDoor().isUnlocked();
    renderer.drawText(
      is_unlocked ? Localization::text(exit_open_label)
                  : Localization::text(exit_sealed_label),
      left + width - 12,
      19,
      13,
      is_unlocked ? Palette::success : Palette::muted_text,
      TextAlignEnum::right
    );

    const auto &all_generators = generators.all();

    for(std::size_t index = 0; index < all_generators.size(); ++index) {
      const GeneratorEntity &generator = all_generators[index];
      const float      chip_x = left + 16 + (static_cast<float>(index) * 33);
      const ColorProps color  = generator.statusColor();

      renderer.drawUiRectangle(chip_x, 46, 27, 24, color.withAlpha(0.12F));
      renderer.drawUiRectangle(
        chip_x,
        68,
        27 * (generator.isOnline() ? 1.0F : generator.progress()),
        2,
        color
      );

      renderer.drawText(
        std::string(1, generator.label()),
        chip_x + 13.5F,
        48,
        16,
        color,
        TextAlignEnum::center
      );
    }
  }
} // namespace wheel_of_fear
