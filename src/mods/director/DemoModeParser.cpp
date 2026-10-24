#include "mods/director/DemoModeParser.hpp"
#include <array>
#include <cstdlib>
#include <utility>

namespace wheel_of_fear {
  DemoModeEnum DemoModeParser::parse(std::string_view name) {
    constexpr std::array<std::pair<std::string_view, DemoModeEnum>, 16> modes {
      {
       {"none", DemoModeEnum::none},
       {"play", DemoModeEnum::play},
       {"deep", DemoModeEnum::deep},
       {"tracker", DemoModeEnum::tracker},
       {"sectors", DemoModeEnum::sectors},
       {"showcase", DemoModeEnum::showcase},
       {"freeze", DemoModeEnum::freeze},
       {"moth", DemoModeEnum::moth},
       {"locker", DemoModeEnum::locker},
       {"witness", DemoModeEnum::witness},
       {"repair", DemoModeEnum::repair},
       {"power", DemoModeEnum::power},
       {"grenade", DemoModeEnum::grenade},
       {"lure", DemoModeEnum::lure},
       {"doors", DemoModeEnum::doors},
       {"walk", DemoModeEnum::walk},
       },
    };

    for(const auto &[mode_name, mode] : modes)
      if(mode_name == name) return mode;

    return DemoModeEnum::none;
  }

  DemoModeEnum DemoModeParser::fromEnvironment() {
    const char* value = std::getenv("ENGRENAGEM_DEMO");
    if(value == nullptr) return DemoModeEnum::none;
    return parse(value);
  }
} // namespace wheel_of_fear
