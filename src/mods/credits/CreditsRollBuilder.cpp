#include "mods/credits/CreditsRollBuilder.hpp"
#include "core/text/Localization.hpp"
#include "core/theme/Palette.hpp"
#include "mods/credits/CreditsCatalog.hpp"
#include <format>

namespace wheel_of_fear {
  namespace {
    constexpr LocalizedTextProps escaped_line {
      "YOU MADE IT OUT",
      "VOCÊ CONSEGUIU SAIR",
    };
    constexpr LocalizedTextProps developed_by_line {
      "DEVELOPED BY",
      "DESENVOLVIDO POR",
    };
    constexpr LocalizedTextProps assets_line {
      "AUDIO & ASSETS",
      "ÁUDIO E ASSETS",
    };
    constexpr LocalizedTextProps thanks_line {
      "THANK YOU FOR PLAYING",
      "OBRIGADO POR JOGAR",
    };
  } // namespace

  std::vector<CreditLineProps> CreditsRollBuilder::build(
    std::span<const CreditEntryProps> entries,
    CreditsRollStyleEnum              style
  ) {
    std::vector<CreditLineProps> lines {
      {
       .text          = "WHEEL OF FEAR",
       .size          = 44,
       .color         = Palette::neon_cyan,
       .is_display    = true,
       .spacing_after = 70,
       },
      {
       .text          = std::string(Localization::text(developed_by_line)),
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
       .text          = std::string(Localization::text(assets_line)),
       .size          = 14,
       .color         = Palette::muted_text,
       .spacing_after = 14,
       },
    };

    if(style == CreditsRollStyleEnum::ending) {
      lines.front().spacing_after = 14;
      lines.insert(
        lines.begin() + 1,
        CreditLineProps {
          .text          = std::string(Localization::text(escaped_line)),
          .size          = 18,
          .color         = Palette::accent,
          .spacing_after = 70,
        }
      );
    }

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
        .text       = std::string(Localization::text(thanks_line)),
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
