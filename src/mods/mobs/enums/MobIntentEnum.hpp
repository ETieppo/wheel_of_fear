#pragma once

#include <cstdint>
#include <string_view>

namespace wheel_of_fear {
  enum class MobIntentEnum : std::uint8_t {
    patrolling,
    investigating,
    chasing,
    searching,
    retreating,
    lurking,
    calling,
    lured,
  };

  constexpr std::string_view nameOf(MobIntentEnum intent) {
    switch(intent) {
      case MobIntentEnum::patrolling:    return "patrolling";
      case MobIntentEnum::investigating: return "investigating";
      case MobIntentEnum::chasing:       return "chasing";
      case MobIntentEnum::searching:     return "searching";
      case MobIntentEnum::retreating:    return "retreating";
      case MobIntentEnum::lurking:       return "lurking";
      case MobIntentEnum::calling:       return "calling";
      case MobIntentEnum::lured:         return "lured";
    }

    return "";
  }
} // namespace wheel_of_fear
