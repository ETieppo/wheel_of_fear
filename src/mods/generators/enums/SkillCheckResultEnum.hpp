#pragma once

#include <cstdint>
#include <string_view>

namespace wheel_of_fear {
  enum class SkillCheckResultEnum : std::uint8_t {
    none,
    perfect,
    good,
    failed,
  };

  constexpr std::string_view nameOf(
    SkillCheckResultEnum value
  ) {
    switch(value) {
      case SkillCheckResultEnum::none:    return "none";
      case SkillCheckResultEnum::perfect: return "perfect";
      case SkillCheckResultEnum::good:    return "good";
      case SkillCheckResultEnum::failed:  return "failed";
    }

    return "";
  }
} // namespace wheel_of_fear
