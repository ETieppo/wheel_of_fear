#include "mods/hud/widgets/ObjectivesPanelWidget.hpp"

#include <format>
#include <string>

#include "core/render/RenderConfig.hpp"
#include "core/render/UiPainter.hpp"
#include "core/theme/Palette.hpp"
#include "mods/generators/GeneratorSystem.hpp"

namespace wheel_of_fear {
  void ObjectivesPanelWidget::draw(
    Renderer              &renderer,
    std::uint32_t          phase,
    const GeneratorSystem &generators
  ) {
    constexpr float width = 220;
    constexpr float left  = RenderConfig::ui_width - 14 - width;

    UiPainter::drawPanel(
      renderer,
      left,
      14,
      width,
      70,
      Palette::hud_magenta,
      1
    );

    renderer.drawText(
      std::format("SECTOR {:02}", phase),
      left + 16,
      19,
      15,
      Palette::hud_magenta,
      TextAlignEnum::left
    );

    const bool is_unlocked = generators.exitDoor().isUnlocked();

    renderer.drawText(
      is_unlocked ? "EXIT OPEN" : "EXIT SEALED",
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
