#include "mods/director/DemoModeParser.hpp"
#include "core/text/TextNumberParser.hpp"
#include "mods/director/CampaignConfig.hpp"
#include <array>
#include <cstdlib>
#include <utility>

namespace wheel_of_fear {
  DemoModeEnum DemoModeParser::parse(std::string_view name) {
    constexpr std::array<std::pair<std::string_view, DemoModeEnum>, 18> modes {
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
       {"phase", DemoModeEnum::phase},
       {"power", DemoModeEnum::power},
       {"grenade", DemoModeEnum::grenade},
       {"lure", DemoModeEnum::lure},
       {"doors", DemoModeEnum::doors},
       {"walk", DemoModeEnum::walk},
       {"ending", DemoModeEnum::ending},
       },
    };

    for(const auto &[mode_name, mode] : modes)
      if(mode_name == name) return mode;

    return DemoModeEnum::none;
  }

  std::optional<std::uint32_t>
  DemoModeParser::parsePhase(std::string_view text) {
    const std::optional<std::uint32_t> phase
      = TextNumberParser::parse<std::uint32_t>(text);
    if(!phase || *phase == 0) return std::nullopt;
    return CampaignConfig::clampPhase(*phase);
  }

  DemoModeProps DemoModeParser::fromEnvironment() {
    const char* mode_value  = std::getenv("ENGRENAGEM_DEMO");
    const char* phase_value = std::getenv("ENGRENAGEM_PHASE");
    if(mode_value == nullptr) return DemoModeProps {};

    return DemoModeProps {
      .mode = parse(mode_value),
      .starting_phase
      = phase_value == nullptr ? std::nullopt : parsePhase(phase_value),
    };
  }
} // namespace wheel_of_fear
