#pragma once

#include "core/storage/TextFileStorage.hpp"
#include "core/text/enums/LanguageEnum.hpp"
#include <filesystem>
#include <optional>
#include <string_view>

namespace wheel_of_fear {
  class SettingsStore {
    public:
      explicit SettingsStore(std::optional<std::filesystem::path> file_path);

      [[nodiscard]]
      LanguageEnum language() const {
        return language_;
      }

      void saveLanguage(LanguageEnum language);

    private:
      static constexpr std::string_view language_key = "language";

      static std::optional<LanguageEnum> parseLanguage(std::string_view text);

      std::optional<TextFileStorage> storage_;
      LanguageEnum                   language_ = LanguageEnum::english;
  };
} // namespace wheel_of_fear
