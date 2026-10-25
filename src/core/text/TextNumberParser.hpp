#pragma once

#include <charconv>
#include <concepts>
#include <optional>
#include <string_view>
#include <system_error>

namespace wheel_of_fear {
  class TextNumberParser final {
    public:
      TextNumberParser() = delete;

      template<std::unsigned_integral NumberType>
      static std::optional<NumberType> parse(std::string_view text) {
        NumberType  number   = 0;
        const char* text_end = text.data() + text.size();
        const auto [parsed_end, error]
          = std::from_chars(text.data(), text_end, number);

        if(error != std::errc {} || parsed_end != text_end || text.empty())
          return std::nullopt;

        return number;
      }
  };
} // namespace wheel_of_fear
