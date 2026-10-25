#include "mods/settings/SettingsStore.hpp"
#include "core/text/Localization.hpp"
#include <format>
#include <string>
#include <utility>

namespace wheel_of_fear {
  SettingsStore::SettingsStore(std::optional<std::filesystem::path> file_path) {
    if(!file_path) return;

    storage_.emplace(std::move(*file_path));
    if(const std::optional<std::string> text = storage_->read())
      language_ = parseLanguage(*text).value_or(LanguageEnum::english);
  }

  void SettingsStore::saveLanguage(LanguageEnum language) {
    language_ = language;
    if(!storage_) return;
    storage_->write(std::format("{}={}\n", language_key, codeOf(language)));
  }

  std::optional<LanguageEnum>
  SettingsStore::parseLanguage(std::string_view text) {
    while(!text.empty()) {
      const std::size_t line_end = text.find('\n');
      std::string_view  line     = text.substr(0, line_end);
      text = line_end == std::string_view::npos ? std::string_view {}
                                                : text.substr(line_end + 1);

      if(line.ends_with('\r')) line.remove_suffix(1);
      const std::size_t separator = line.find('=');
      if(separator == std::string_view::npos) continue;
      if(line.substr(0, separator) != language_key) continue;

      return Localization::parse(line.substr(separator + 1));
    }

    return std::nullopt;
  }
} // namespace wheel_of_fear
