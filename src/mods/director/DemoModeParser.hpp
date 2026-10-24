#pragma once

#include "mods/director/enums/DemoModeEnum.hpp"
#include <string_view>

namespace wheel_of_fear {
  class DemoModeParser final {
    public:
      DemoModeParser() = delete;

      static DemoModeEnum parse(std::string_view name);
      static DemoModeEnum fromEnvironment();
  };
} // namespace wheel_of_fear
