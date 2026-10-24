#pragma once

#include <cstdint>
#include <string_view>

namespace wheel_of_fear {
  enum class GeneratorStateEnum : std::uint8_t {
    offline,
    charging,
    jammed,
    broken,
    online,
  };

  constexpr std::string_view nameOf(GeneratorStateEnum value) {
    switch(value) {
      case GeneratorStateEnum::offline:  return "offline";
      case GeneratorStateEnum::charging: return "charging";
      case GeneratorStateEnum::jammed:   return "jammed";
      case GeneratorStateEnum::broken:   return "broken";
      case GeneratorStateEnum::online:   return "online";
    }

    return "";
  }
} // namespace wheel_of_fear
