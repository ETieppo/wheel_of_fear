#pragma once

#include <cstdint>

namespace wheel_of_fear {
  enum class GameStateEnum : std::uint8_t {
    title,
    phase_intro,
    playing,
    paused,
    game_over,
    phase_cleared,
    credits,
    ending,
  };
} // namespace wheel_of_fear
