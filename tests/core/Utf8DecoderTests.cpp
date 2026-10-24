#include <array>
#include <string_view>

#include <doctest/doctest.h>

#include "core/render/Utf8Decoder.hpp"

namespace wheel_of_fear {
  TEST_CASE(
    "decodes latin-1 accents"
  ) {
    constexpr std::string_view text   = "a\xC3\xA7\xC3\xA3o";
    std::size_t                cursor = 0;
    std::array<char32_t, 8>    decoded {};
    std::size_t                count = 0;
    while(cursor < text.size())
      decoded[count++] = Utf8Decoder::nextCodepoint(text, cursor);
    CHECK(count == 4);
    CHECK(decoded[1] == 0xE7);
  }
} // namespace wheel_of_fear
