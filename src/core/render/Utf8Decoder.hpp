#pragma once

#include <cstddef>
#include <string_view>

namespace wheel_of_fear {
  class Utf8Decoder final {
    public:
      Utf8Decoder() = delete;

      static char32_t nextCodepoint(
        std::string_view text,
        std::size_t     &cursor
      );
  };
} // namespace wheel_of_fear
