#pragma once

#include "core/text/enums/LanguageEnum.hpp"
#include <cstddef>
#include <string_view>

namespace wheel_of_fear {
  struct LocalizedTextProps {
      std::string_view english;
      std::string_view portuguese;

      constexpr LocalizedTextProps() = default;

      consteval LocalizedTextProps(
        std::string_view english_text,
        std::string_view portuguese_text
      )
          : english(english_text)
          , portuguese(portuguese_text) {
        if(!haveSamePlaceholders(english_text, portuguese_text))
          throw "localized texts must use the same format placeholders";
      }

      [[nodiscard]]
      constexpr std::string_view in(LanguageEnum language) const {
        return language == LanguageEnum::brazilian_portuguese ? portuguese
                                                              : english;
      }

    private:
      static constexpr std::string_view
      nextPlaceholder(std::string_view text, std::size_t &cursor) {
        while(cursor < text.size()) {
          const std::size_t open = text.find('{', cursor);
          if(open == std::string_view::npos) break;

          const std::size_t close = text.find('}', open);
          if(close == std::string_view::npos) break;

          cursor = close + 1;
          return text.substr(open, close - open + 1);
        }

        cursor = text.size();
        return {};
      }

      static constexpr bool haveSamePlaceholders(
        std::string_view first,
        std::string_view second
      ) {
        std::size_t first_cursor  = 0;
        std::size_t second_cursor = 0;

        while(true) {
          const std::string_view first_placeholder
            = nextPlaceholder(first, first_cursor);
          const std::string_view second_placeholder
            = nextPlaceholder(second, second_cursor);

          if(first_placeholder != second_placeholder) return false;
          if(first_placeholder.empty()) return true;
        }
      }
  };
} // namespace wheel_of_fear
