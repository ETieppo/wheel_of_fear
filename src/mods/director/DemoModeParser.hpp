#pragma once

#include "mods/director/DemoModeProps.hpp"
#include "mods/director/enums/DemoModeEnum.hpp"
#include <cstdint>
#include <optional>
#include <string_view>

namespace wheel_of_fear {
  class DemoModeParser final {
    public:
      DemoModeParser() = delete;

      static DemoModeEnum                 parse(std::string_view name);
      static std::optional<std::uint32_t> parsePhase(std::string_view text);
      static DemoModeProps                fromEnvironment();
  };
} // namespace wheel_of_fear
