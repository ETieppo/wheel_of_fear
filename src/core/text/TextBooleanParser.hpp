#pragma once

#include <algorithm>
#include <array>
#include <cctype>
#include <optional>
#include <string>
#include <string_view>

namespace wheel_of_fear {
  class TextBooleanParser final {
    public:
      TextBooleanParser() = delete;

      static std::optional<bool> parse(std::string_view text) {
        constexpr std::array<std::string_view, 4> truthy_words {
          "1",
          "true",
          "yes",
          "on",
        };
        constexpr std::array<std::string_view, 4> falsy_words {
          "0",
          "false",
          "no",
          "off",
        };

        std::string lowered(text);
        std::ranges::transform(lowered, lowered.begin(), [](unsigned char c) {
          return static_cast<char>(std::tolower(c));
        });

        if(std::ranges::contains(truthy_words, lowered)) return true;
        if(std::ranges::contains(falsy_words, lowered)) return false;
        return std::nullopt;
      }
  };
} // namespace wheel_of_fear
