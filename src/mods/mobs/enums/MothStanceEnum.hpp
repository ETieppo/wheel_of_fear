#pragma once

#include <cstdint>
#include <string_view>

namespace wheel_of_fear {
  enum class MothStanceEnum : std::uint8_t {
    waiting,
    following,
    departing,
  };

  constexpr std::string_view nameOf(
    MothStanceEnum stance
  ) {
    switch(stance) {
      case MothStanceEnum::waiting:   return "waiting";
      case MothStanceEnum::following: return "following";
      case MothStanceEnum::departing: return "departing";
    }

    return "";
  }
} // namespace wheel_of_fear
