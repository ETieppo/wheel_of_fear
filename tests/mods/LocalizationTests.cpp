#include "core/text/Localization.hpp"
#include "mods/letters/LetterCatalog.hpp"
#include "mods/settings/SettingsStore.hpp"
#include <doctest/doctest.h>
#include <filesystem>

namespace wheel_of_fear {
  namespace {
    class LanguageGuard {
      public:
        explicit LanguageGuard(LanguageEnum language) {
          Localization::setLanguage(language);
        }

        LanguageGuard(const LanguageGuard &)             = delete;
        LanguageGuard &operator =(const LanguageGuard &) = delete;

        ~LanguageGuard() {
          Localization::setLanguage(LanguageEnum::english);
        }
    };

    constexpr LocalizedTextProps sector_label {"SECTOR {:02}", "SETOR {:02}"};

    std::filesystem::path freshSettingsPath() {
      const std::filesystem::path path = std::filesystem::temp_directory_path()
        / "wheel_of_fear_settings_tests.cfg";
      std::filesystem::remove(path);
      return path;
    }
  } // namespace

  TEST_CASE("localized texts follow the selected language") {
    CHECK(Localization::format(sector_label, 3) == "SECTOR 03");

    const LanguageGuard guard(LanguageEnum::brazilian_portuguese);
    CHECK(Localization::format(sector_label, 3) == "SETOR 03");
  }

  TEST_CASE("the chosen language survives a restart") {
    const std::filesystem::path path = freshSettingsPath();
    SettingsStore {path}.saveLanguage(LanguageEnum::brazilian_portuguese);

    CHECK(
      SettingsStore {path}.language() == LanguageEnum::brazilian_portuguese
    );
    std::filesystem::remove(path);
  }

  TEST_CASE("settings without a file default to english and never persist") {
    SettingsStore settings {std::nullopt};
    settings.saveLanguage(LanguageEnum::brazilian_portuguese);

    CHECK(SettingsStore {std::nullopt}.language() == LanguageEnum::english);
  }

  TEST_CASE("letter files are split into one letter per section") {
    const auto letters = LetterCatalog::parse(
      "# Title\n\nintro text\n\n## 1\nfirst line\nsecond line\n\n## 2\nlast\n"
    );

    REQUIRE(letters.size() == 2);
    CHECK(letters[0] == "first line second line");
    CHECK(letters[1] == "last");
  }
} // namespace wheel_of_fear
