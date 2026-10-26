#pragma once

#include "mods/director/DeveloperOptionsProps.hpp"

namespace wheel_of_fear {
  class DeveloperOptionsParser final {
    public:
      DeveloperOptionsParser() = delete;

      static DeveloperOptionsProps parse(
        const char* god_mode_value,
        const char* language_value
      );

      static DeveloperOptionsProps fromEnvironment();
  };
} // namespace wheel_of_fear
