#pragma once

#include <cstdint>

namespace wheel_of_fear {
  enum class NoiseSourceEnum : std::uint8_t {
    footstep,
    running,
    repairing,
    repair_failure,
    fabricating,
    lure_beacon,
    throwable_impact,
    explosion,
    breathing,
    tracker_beep,
    locker_door,
    maze_shift,
    mob_incident,
    reinforcement_call,
    shepherd_call,
  };
} // namespace wheel_of_fear
