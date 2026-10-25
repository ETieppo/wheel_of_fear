#include "mods/letters/LetterCatalog.hpp"
#include "core/assets/AssetLocator.hpp"
#include <SDL3/SDL_log.h>
#include <algorithm>
#include <exception>
#include <format>

namespace wheel_of_fear {
  std::vector<std::string> LetterCatalog::load(LanguageEnum language) {
    try {
      return parse(
        AssetLocator::readText(
          AssetLocator::assetPath(
            std::format("letters/{}.md", codeOf(language))
          )
        )
      );
    } catch(const std::exception &error) {
      SDL_Log("letters unavailable: %s", error.what());
      return {};
    }
  }

  std::vector<std::string> LetterCatalog::parse(std::string_view markdown) {
    std::vector<std::string> letters;
    bool                     is_inside_letter = false;

    while(!markdown.empty()) {
      const std::size_t line_end = markdown.find('\n');
      std::string_view  line     = markdown.substr(0, line_end);
      markdown                   = line_end == std::string_view::npos
        ? std::string_view {}
        : markdown.substr(line_end + 1);

      if(line.ends_with('\r')) line.remove_suffix(1);

      if(line.starts_with(section_marker)) {
        letters.emplace_back();
        is_inside_letter = true;
        continue;
      }

      if(!is_inside_letter || line.empty()) continue;

      std::string &letter = letters.back();
      if(!letter.empty()) letter += ' ';
      letter += line;
    }

    return letters;
  }

  const LocalizedTextProps &LetterCatalog::giftNoteFor(std::uint32_t phase) {
    return gift_notes[(std::max(phase, 1U) - 1) % gift_notes.size()];
  }

  LetterRewardProps LetterCatalog::rewardFor(std::uint32_t phase) {
    if(phase % 4 == 0)
      return LetterRewardProps {.item = ItemEnum::lure_beacon, .count = 2};
    if(phase % 2 == 0)
      return LetterRewardProps {.item = ItemEnum::lure_beacon, .count = 1};
    return LetterRewardProps {.item = ItemEnum::neon_grenade, .count = 1};
  }
} // namespace wheel_of_fear
