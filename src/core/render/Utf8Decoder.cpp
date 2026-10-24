#include "core/render/Utf8Decoder.hpp"
#include <cstdint>

namespace wheel_of_fear {
  char32_t Utf8Decoder::nextCodepoint(
    std::string_view text,
    std::size_t     &cursor
  ) {
    const auto first = static_cast<std::uint8_t>(text[cursor]);

    if(first < 0x80) {
      cursor += 1;
      return first;
    }

    std::size_t sequence_length = 4;
    char32_t    codepoint       = first & 0x07U;

    if((first & 0xE0U) == 0xC0U) {
      sequence_length = 2;
      codepoint       = first & 0x1FU;
    } else if((first & 0xF0U) == 0xE0U) {
      sequence_length = 3;
      codepoint       = first & 0x0FU;
    }

    if(cursor + sequence_length > text.size()) {
      cursor = text.size();
      return U'?';
    }

    for(std::size_t offset = 1; offset < sequence_length; ++offset) {
      const auto continuation
        = static_cast<std::uint8_t>(text[cursor + offset]);

      if((continuation & 0xC0U) != 0x80U) {
        cursor += sequence_length;
        return U'?';
      }

      codepoint = (codepoint << 6) | (continuation & 0x3FU);
    }

    cursor += sequence_length;
    return codepoint;
  }
} // namespace wheel_of_fear
