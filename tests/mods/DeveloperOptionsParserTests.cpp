#include "mods/director/DeveloperOptionsParser.hpp"
#include <doctest/doctest.h>

namespace wheel_of_fear {
  TEST_CASE("missing developer options keep the defaults") {
    const DeveloperOptionsProps options
      = DeveloperOptionsParser::parse(nullptr, nullptr);

    CHECK_FALSE(options.is_god_mode);
    CHECK_FALSE(options.language.has_value());
  }

  TEST_CASE("god mode follows the boolean value") {
    CHECK(DeveloperOptionsParser::parse("true", nullptr).is_god_mode);
    CHECK(DeveloperOptionsParser::parse("1", nullptr).is_god_mode);
    CHECK_FALSE(DeveloperOptionsParser::parse("false", nullptr).is_god_mode);
    CHECK_FALSE(DeveloperOptionsParser::parse("banana", nullptr).is_god_mode);
  }

  TEST_CASE("language accepts the supported codes only") {
    CHECK(
      DeveloperOptionsParser::parse(nullptr, "en").language
      == LanguageEnum::english
    );
    CHECK(
      DeveloperOptionsParser::parse(nullptr, "pt-BR").language
      == LanguageEnum::brazilian_portuguese
    );
    CHECK_FALSE(
      DeveloperOptionsParser::parse(nullptr, "fr").language.has_value()
    );
  }
} // namespace wheel_of_fear
