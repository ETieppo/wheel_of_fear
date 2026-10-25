#include "mods/credits/CreditsRollBuilder.hpp"
#include "core/theme/Palette.hpp"
#include "mods/credits/CreditsCatalog.hpp"
#include <format>

namespace wheel_of_fear {
  std::vector<CreditLineProps>
  CreditsRollBuilder::build(std::span<const CreditEntryProps> entries) {
    std::vector<CreditLineProps> lines {
      {
       .text          = "WHEEL OF FEAR",
       .size          = 44,
       .color         = Palette::neon_cyan,
       .is_display    = true,
       .spacing_after = 14,
       },
      {
       .text          = "YOU MADE IT OUT",
       .size          = 18,
       .color         = Palette::accent,
       .spacing_after = 70,
       },
      {
       .text          = "DEVELOPED BY",
       .size          = 14,
       .color         = Palette::muted_text,
       .spacing_after = 8,
       },
      {
       .text          = std::string(CreditsCatalog::developer_name),
       .size          = 30,
       .color         = Palette::accent,
       .is_display    = true,
       .spacing_after = 70,
       },
      {
       .text          = "AUDIO & ASSETS",
       .size          = 14,
       .color         = Palette::muted_text,
       .spacing_after = 14,
       },
    };

    for(const CreditEntryProps &entry : entries)
      lines.push_back(
        CreditLineProps {
          .text = std::format(
            "{}  -  {}  -  {}",
            entry.asset,
            entry.author,
            entry.license
          ),
          .size          = 13,
          .color         = Palette::text,
          .spacing_after = 6,
        }
      );

    lines.back().spacing_after = 70;
    lines.push_back(
      CreditLineProps {
        .text       = "THANK YOU FOR PLAYING",
        .size       = 22,
        .color      = Palette::neon_cyan,
        .is_display = true,
      }
    );

    return lines;
  }

  float
  CreditsRollBuilder::totalHeight(std::span<const CreditLineProps> lines) {
    float height = 0;
    for(const CreditLineProps &line : lines)
      height += line.size + line.spacing_after;
    return height;
  }
} // namespace wheel_of_fear
