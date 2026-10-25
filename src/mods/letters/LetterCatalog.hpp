#pragma once

#include "core/text/LocalizedTextProps.hpp"
#include "core/text/enums/LanguageEnum.hpp"
#include "mods/letters/LetterRewardProps.hpp"
#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace wheel_of_fear {
  class LetterCatalog final {
    public:
      LetterCatalog() = delete;

      static constexpr std::array<LocalizedTextProps, 4> gift_notes {
        {
         {
            "Out of guilt, I leave something for you. Maybe it helps.",
            "Pela culpa, deixo algo pra ti. Quem sabe isso te ajude.",
          }, {
            "It isn't much, but it is what I managed to hide.",
            "Não é muito, mas foi o que consegui esconder.",
          }, {
            "Take this. I won't be needing it anymore.",
            "Leve isto. Eu não vou mais precisar.",
          }, {
            "I kept this for whoever came next. I hope it is you.",
            "Guardei isto pra quem viesse depois. Espero que seja você.",
          }, },
      };

      static std::vector<std::string> load(LanguageEnum language);
      static std::vector<std::string> parse(std::string_view markdown);

      [[nodiscard]]
      static const LocalizedTextProps &giftNoteFor(std::uint32_t phase);

      [[nodiscard]]
      static LetterRewardProps rewardFor(std::uint32_t phase);

    private:
      static constexpr std::string_view section_marker = "## ";
  };
} // namespace wheel_of_fear
