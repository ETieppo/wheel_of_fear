#pragma once

#include <cstdint>
#include <string_view>

namespace wheel_of_fear {
  enum class LanguageEnum : std::uint8_t {
    english,
    brazilian_portuguese,
  };

  constexpr std::string_view codeOf(LanguageEnum language) {
    switch(language) {
      case LanguageEnum::english:              return "en";
      case LanguageEnum::brazilian_portuguese: return "pt-BR";
    }

    return "en";
  }
} // namespace wheel_of_fear
