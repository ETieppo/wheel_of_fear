#include "mods/director/DeveloperOptionsParser.hpp"
#include "core/text/Localization.hpp"
#include "core/text/TextBooleanParser.hpp"
#include <cstdlib>

namespace wheel_of_fear {
  DeveloperOptionsProps DeveloperOptionsParser::parse(
    const char* god_mode_value,
    const char* language_value
  ) {
    return DeveloperOptionsProps {
      .is_god_mode = god_mode_value != nullptr
        && TextBooleanParser::parse(god_mode_value).value_or(false),
      .language = language_value == nullptr
        ? std::nullopt
        : Localization::parse(language_value),
    };
  }

  DeveloperOptionsProps DeveloperOptionsParser::fromEnvironment() {
    return parse(
      std::getenv("ENGRENAGEM_GOD"),
      std::getenv("ENGRENAGEM_LANG")
    );
  }
} // namespace wheel_of_fear
