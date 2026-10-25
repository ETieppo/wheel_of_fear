#pragma once

#include <cstddef>
#include <cstdint>

namespace wheel_of_fear {
  enum class InputActionEnum : std::uint8_t {
    move_up,
    move_down,
    move_left,
    move_right,
    run,
    interact,
    hold_breath,
    throw_lure,
    throw_grenade,
    toggle_tracker,
    switch_light_mode,
    hug,
    confirm,
    back,
    restart,
    toggle_mute,
    show_credits,
    quit_game,
  };

  inline constexpr std::size_t input_action_count = 18;
} // namespace wheel_of_fear
