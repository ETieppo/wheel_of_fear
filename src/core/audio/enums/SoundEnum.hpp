#pragma once

#include <cstddef>
#include <cstdint>

namespace wheel_of_fear {
  enum class SoundEnum : std::uint8_t {
    footstep,
    crawler_skitter,
    crawler_chitter,
    crumble,
    explosion,
    tracker_open,
    tracker_close,
    tracker_blip,
    generator_tick,
    generator_online,
    generator_breakdown,
    generator_request,
    part_pickup,
    door_open,
    player_hurt,
    heartbeat,
    machine_work,
    switch_click,
    acid_sizzle,
    shepherd_call,
    shepherd_step,
    moth_flutter,
    player_death,
    phase_start,
    whisper,
    jaws,
    electric_buzz,
    reinforcement_whistle,
    metal_clang,
    distant_scream,
    glass_shatter,
    decoy_throw,
    decoy_impact,
    locker_door,
    mob_burn,
    breathing,
    bloater_gurgle,
    melter_slither,
    mimic_clank,
    blast_door,
    led_power_up,
    qte_alert,
    qte_success,
    qte_fail,
    neon_burst,
    lure_beep,
    battery_charge,
    ui_glitch,
  };

  inline constexpr std::size_t sound_count = 48;
} // namespace wheel_of_fear
