#include "mods/director/screens/CreditsScreen.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/UiPainter.hpp"
#include "core/theme/Palette.hpp"
#include "mods/credits/CreditsCatalog.hpp"
#include <cmath>
#include <format>

namespace wheel_of_fear {
  void CreditsScreen::draw(
    Renderer                         &renderer,
    std::span<const CreditEntryProps> entries,
    float                             real_seconds
  ) {
    constexpr float center_x = RenderConfig::ui_width / 2;

    renderer.drawUiRectangle(
      0,
      0,
      RenderConfig::ui_width,
      RenderConfig::ui_height,
      Palette::panel_deep.withAlpha(0.85F)
    );

    renderer.drawDisplayText(
      "CREDITS",
      center_x,
      34,
      34,
      Palette::neon_cyan,
      TextAlignEnum::center
    );

    renderer.drawText(
      "DEVELOPED BY",
      center_x,
      100,
      14,
      Palette::muted_text,
      TextAlignEnum::center
    );

    renderer.drawDisplayText(
      CreditsCatalog::developer_name,
      center_x,
      120,
      30,
      Palette::accent,
      TextAlignEnum::center
    );

    renderer.drawText(
      "AUDIO & ASSETS",
      center_x,
      190,
      14,
      Palette::muted_text,
      TextAlignEnum::center
    );

    drawEntries(renderer, entries);

    renderer.drawText(
      "procedurally generated creatures, machinery and their sounds need no "
      "attribution",
      center_x,
      470,
      12,
      Palette::muted_text,
      TextAlignEnum::center
    );

    const float pulse = 0.45F + (0.55F * std::sin(real_seconds * 3));

    renderer.drawDisplayText(
      "PRESS ESC TO RETURN",
      center_x,
      500,
      18,
      Palette::accent.withAlpha(pulse),
      TextAlignEnum::center
    );

    UiPainter::drawScanlines(
      renderer,
      RenderConfig::ui_width,
      RenderConfig::ui_height,
      0.12F
    );
  }

  void CreditsScreen::drawEntries(
    Renderer                         &renderer,
    std::span<const CreditEntryProps> entries
  ) {
    const std::size_t rows_per_column = (entries.size() + 1) / 2;
    constexpr float   left_column_x
      = (RenderConfig::ui_width / 2) - entry_column_width;

    for(std::size_t index = 0; index < entries.size(); ++index) {
      const CreditEntryProps &entry  = entries[index];
      const std::size_t       column = index / rows_per_column;
      const std::size_t       row    = index % rows_per_column;
      const float             x      = left_column_x
        + (static_cast<float>(column) * entry_column_width)
        + 20;
      const float y
        = entry_section_top + (static_cast<float>(row) * entry_row_height);

      renderer
        .drawText(entry.asset, x, y, 12, Palette::text, TextAlignEnum::left);

      renderer.drawText(
        std::format("{}  -  {}", entry.author, entry.license),
        x + entry_author_offset,
        y,
        12,
        Palette::muted_text,
        TextAlignEnum::left
      );
    }
  }
} // namespace wheel_of_fear
