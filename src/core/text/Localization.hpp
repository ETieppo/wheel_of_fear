#pragma once

#include "core/text/LocalizedTextProps.hpp"
#include "core/text/enums/LanguageEnum.hpp"
#include <format>
#include <optional>
#include <string>
#include <string_view>

namespace wheel_of_fear {
  class Localization final {
    public:
      Localization() = delete;

      [[nodiscard]]
      static LanguageEnum language() {
        return language_;
      }

      static void setLanguage(LanguageEnum language) {
        language_ = language;
      }

      static void toggleLanguage() {
        language_ = language_ == LanguageEnum::english
          ? LanguageEnum::brazilian_portuguese
          : LanguageEnum::english;
      }

      [[nodiscard]]
      static std::string_view text(const LocalizedTextProps &localized) {
        return localized.in(language_);
      }

      template<typename... ArgumentTypes> [[nodiscard]]
      static std::string format(
        const LocalizedTextProps &localized,
        const ArgumentTypes &... arguments
      ) {
        return std::vformat(
          text(localized),
          std::make_format_args(arguments...)
        );
      }

      [[nodiscard]]
      static std::optional<LanguageEnum> parse(std::string_view code) {
        if(code == codeOf(LanguageEnum::english)) return LanguageEnum::english;
        if(code == codeOf(LanguageEnum::brazilian_portuguese))
          return LanguageEnum::brazilian_portuguese;
        return std::nullopt;
      }

    private:
      static inline LanguageEnum language_ = LanguageEnum::english;
  };
} // namespace wheel_of_fear
