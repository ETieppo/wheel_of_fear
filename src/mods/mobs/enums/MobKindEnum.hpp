#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace wheel_of_fear {
  enum class MobKindEnum : std::uint8_t {
    crawler,
    bloater,
    melter,
    shepherd,
    shade,
    mimic,
    moth,
    watcher,
    devourer,
  };

  inline constexpr std::array<MobKindEnum, 9> all_mob_kinds {
    MobKindEnum::crawler,
    MobKindEnum::bloater,
    MobKindEnum::melter,
    MobKindEnum::shepherd,
    MobKindEnum::shade,
    MobKindEnum::mimic,
    MobKindEnum::moth,
    MobKindEnum::watcher,
    MobKindEnum::devourer,
  };

  constexpr std::string_view nameOf(MobKindEnum value) {
    switch(value) {
      case MobKindEnum::crawler:  return "crawler";
      case MobKindEnum::bloater:  return "bloater";
      case MobKindEnum::melter:   return "melter";
      case MobKindEnum::shepherd: return "shepherd";
      case MobKindEnum::shade:    return "shade";
      case MobKindEnum::mimic:    return "mimic";
      case MobKindEnum::moth:     return "moth";
      case MobKindEnum::watcher:  return "watcher";
      case MobKindEnum::devourer: return "devourer";
    }

    return "";
  }
} // namespace wheel_of_fear
