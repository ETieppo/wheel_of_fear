#pragma once

#include "mods/director/enums/DemoModeEnum.hpp"
#include <cstdint>
#include <optional>

namespace wheel_of_fear {
  struct DemoModeProps {
      DemoModeEnum                 mode           = DemoModeEnum::none;
      std::optional<std::uint32_t> starting_phase = std::nullopt;
  };
} // namespace wheel_of_fear
