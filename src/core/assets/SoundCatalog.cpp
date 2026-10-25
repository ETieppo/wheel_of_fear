#include "core/assets/SoundCatalog.hpp"
#include <array>

namespace wheel_of_fear {
  namespace {
    constexpr std::array<std::string_view, 5> footstep_files {
      "audio/sfx/footstep_0.opus",
      "audio/sfx/footstep_1.opus",
      "audio/sfx/footstep_2.opus",
      "audio/sfx/footstep_3.opus",
      "audio/sfx/footstep_4.opus",
    };
    constexpr std::array<std::string_view, 4> crawler_skitter_files {
      "audio/sfx/crawler_skitter_0.opus",
      "audio/sfx/crawler_skitter_1.opus",
      "audio/sfx/crawler_skitter_2.opus",
      "audio/sfx/crawler_skitter_3.opus",
    };
    constexpr std::array<std::string_view, 3> crawler_chitter_files {
      "audio/sfx/crawler_chitter_0.opus",
      "audio/sfx/crawler_chitter_1.opus",
      "audio/sfx/crawler_chitter_2.opus",
    };
    constexpr std::array<std::string_view, 5> crumble_files {
      "audio/sfx/crumble_0.opus",
      "audio/sfx/crumble_1.opus",
      "audio/sfx/crumble_2.opus",
      "audio/sfx/crumble_3.opus",
      "audio/sfx/crumble_4.opus",
    };
    constexpr std::array<std::string_view, 5> explosion_files {
      "audio/sfx/explosion_0.opus",
      "audio/sfx/explosion_1.opus",
      "audio/sfx/explosion_2.opus",
      "audio/sfx/explosion_3.opus",
      "audio/sfx/explosion_4.opus",
    };
    constexpr std::array<std::string_view, 4> tracker_open_files {
      "audio/sfx/tracker_open_0.opus",
      "audio/sfx/tracker_open_1.opus",
      "audio/sfx/tracker_open_2.opus",
      "audio/sfx/tracker_open_3.opus",
    };
    constexpr std::array<std::string_view, 4> tracker_close_files {
      "audio/sfx/tracker_close_0.opus",
      "audio/sfx/tracker_close_1.opus",
      "audio/sfx/tracker_close_2.opus",
      "audio/sfx/tracker_close_3.opus",
    };
    constexpr std::array<std::string_view, 4> tracker_blip_files {
      "audio/sfx/tracker_blip_0.opus",
      "audio/sfx/tracker_blip_1.opus",
      "audio/sfx/tracker_blip_2.opus",
      "audio/sfx/tracker_blip_3.opus",
    };
    constexpr std::array<std::string_view, 5> generator_tick_files {
      "audio/sfx/generator_tick_0.opus",
      "audio/sfx/generator_tick_1.opus",
      "audio/sfx/generator_tick_2.opus",
      "audio/sfx/generator_tick_3.opus",
      "audio/sfx/generator_tick_4.opus",
    };
    constexpr std::array<std::string_view, 3> generator_online_files {
      "audio/sfx/generator_online_0.opus",
      "audio/sfx/generator_online_1.opus",
      "audio/sfx/generator_online_2.opus",
    };
    constexpr std::array<std::string_view, 5> generator_breakdown_files {
      "audio/sfx/generator_breakdown_0.opus",
      "audio/sfx/generator_breakdown_1.opus",
      "audio/sfx/generator_breakdown_2.opus",
      "audio/sfx/generator_breakdown_3.opus",
      "audio/sfx/generator_breakdown_4.opus",
    };
    constexpr std::array<std::string_view, 4> generator_request_files {
      "audio/sfx/generator_request_0.opus",
      "audio/sfx/generator_request_1.opus",
      "audio/sfx/generator_request_2.opus",
      "audio/sfx/generator_request_3.opus",
    };
    constexpr std::array<std::string_view, 4> part_pickup_files {
      "audio/sfx/part_pickup_0.opus",
      "audio/sfx/part_pickup_1.opus",
      "audio/sfx/part_pickup_2.opus",
      "audio/sfx/part_pickup_3.opus",
    };
    constexpr std::array<std::string_view, 3> door_open_files {
      "audio/sfx/door_open_0.opus",
      "audio/sfx/door_open_1.opus",
      "audio/sfx/door_open_2.opus",
    };
    constexpr std::array<std::string_view, 5> player_hurt_files {
      "audio/sfx/player_hurt_0.opus",
      "audio/sfx/player_hurt_1.opus",
      "audio/sfx/player_hurt_2.opus",
      "audio/sfx/player_hurt_3.opus",
      "audio/sfx/player_hurt_4.opus",
    };
    constexpr std::array<std::string_view, 1> heartbeat_files {
      "audio/sfx/heartbeat_0.opus",
    };
    constexpr std::array<std::string_view, 3> machine_work_files {
      "audio/sfx/machine_work_0.opus",
      "audio/sfx/machine_work_1.opus",
      "audio/sfx/machine_work_2.opus",
    };
    constexpr std::array<std::string_view, 5> switch_click_files {
      "audio/sfx/switch_click_0.opus",
      "audio/sfx/switch_click_1.opus",
      "audio/sfx/switch_click_2.opus",
      "audio/sfx/switch_click_3.opus",
      "audio/sfx/switch_click_4.opus",
    };
    constexpr std::array<std::string_view, 2> acid_sizzle_files {
      "audio/sfx/acid_sizzle_0.opus",
      "audio/sfx/acid_sizzle_1.opus",
    };
    constexpr std::array<std::string_view, 1> shepherd_call_files {
      "audio/sfx/shepherd_call_0.opus",
    };
    constexpr std::array<std::string_view, 3> shepherd_step_files {
      "audio/sfx/shepherd_step_0.opus",
      "audio/sfx/shepherd_step_1.opus",
      "audio/sfx/shepherd_step_2.opus",
    };
    constexpr std::array<std::string_view, 4> moth_flutter_files {
      "audio/sfx/moth_flutter_0.opus",
      "audio/sfx/moth_flutter_1.opus",
      "audio/sfx/moth_flutter_2.opus",
      "audio/sfx/moth_flutter_3.opus",
    };
    constexpr std::array<std::string_view, 1> player_death_files {
      "audio/sfx/player_death_0.opus",
    };
    constexpr std::array<std::string_view, 1> phase_start_files {
      "audio/sfx/phase_start_0.opus",
    };
    constexpr std::array<std::string_view, 2> whisper_files {
      "audio/sfx/whisper_0.opus",
      "audio/sfx/whisper_1.opus",
    };
    constexpr std::array<std::string_view, 5> jaws_files {
      "audio/sfx/jaws_0.opus",
      "audio/sfx/jaws_1.opus",
      "audio/sfx/jaws_2.opus",
      "audio/sfx/jaws_3.opus",
      "audio/sfx/jaws_4.opus",
    };
    constexpr std::array<std::string_view, 4> electric_buzz_files {
      "audio/sfx/electric_buzz_0.opus",
      "audio/sfx/electric_buzz_1.opus",
      "audio/sfx/electric_buzz_2.opus",
      "audio/sfx/electric_buzz_3.opus",
    };
    constexpr std::array<std::string_view, 1> reinforcement_whistle_files {
      "audio/sfx/reinforcement_whistle_0.opus",
    };
    constexpr std::array<std::string_view, 5> metal_clang_files {
      "audio/sfx/metal_clang_0.opus",
      "audio/sfx/metal_clang_1.opus",
      "audio/sfx/metal_clang_2.opus",
      "audio/sfx/metal_clang_3.opus",
      "audio/sfx/metal_clang_4.opus",
    };
    constexpr std::array<std::string_view, 7> distant_scream_files {
      "audio/sfx/distant_scream_0.opus",
      "audio/sfx/distant_scream_1.opus",
      "audio/sfx/distant_scream_2.opus",
      "audio/sfx/distant_scream_3.opus",
      "audio/sfx/distant_scream_4.opus",
      "audio/sfx/distant_scream_5.opus",
      "audio/sfx/distant_scream_6.opus",
    };
    constexpr std::array<std::string_view, 5> glass_shatter_files {
      "audio/sfx/glass_shatter_0.opus",
      "audio/sfx/glass_shatter_1.opus",
      "audio/sfx/glass_shatter_2.opus",
      "audio/sfx/glass_shatter_3.opus",
      "audio/sfx/glass_shatter_4.opus",
    };
    constexpr std::array<std::string_view, 2> decoy_throw_files {
      "audio/sfx/decoy_throw_0.opus",
      "audio/sfx/decoy_throw_1.opus",
    };
    constexpr std::array<std::string_view, 5> decoy_impact_files {
      "audio/sfx/decoy_impact_0.opus",
      "audio/sfx/decoy_impact_1.opus",
      "audio/sfx/decoy_impact_2.opus",
      "audio/sfx/decoy_impact_3.opus",
      "audio/sfx/decoy_impact_4.opus",
    };
    constexpr std::array<std::string_view, 4> locker_door_files {
      "audio/sfx/locker_door_0.opus",
      "audio/sfx/locker_door_1.opus",
      "audio/sfx/locker_door_2.opus",
      "audio/sfx/locker_door_3.opus",
    };
    constexpr std::array<std::string_view, 3> mob_burn_files {
      "audio/sfx/mob_burn_0.opus",
      "audio/sfx/mob_burn_1.opus",
      "audio/sfx/mob_burn_2.opus",
    };
    constexpr std::array<std::string_view, 3> breathing_files {
      "audio/sfx/breathing_0.opus",
      "audio/sfx/breathing_1.opus",
      "audio/sfx/breathing_2.opus",
    };
    constexpr std::array<std::string_view, 6> bloater_gurgle_files {
      "audio/sfx/bloater_gurgle_0.opus",
      "audio/sfx/bloater_gurgle_1.opus",
      "audio/sfx/bloater_gurgle_2.opus",
      "audio/sfx/bloater_gurgle_3.opus",
      "audio/sfx/bloater_gurgle_4.opus",
      "audio/sfx/bloater_gurgle_5.opus",
    };
    constexpr std::array<std::string_view, 3> melter_slither_files {
      "audio/sfx/melter_slither_0.opus",
      "audio/sfx/melter_slither_1.opus",
      "audio/sfx/melter_slither_2.opus",
    };
    constexpr std::array<std::string_view, 3> mimic_clank_files {
      "audio/sfx/mimic_clank_0.opus",
      "audio/sfx/mimic_clank_1.opus",
      "audio/sfx/mimic_clank_2.opus",
    };
    constexpr std::array<std::string_view, 2> blast_door_files {
      "audio/sfx/blast_door_0.opus",
      "audio/sfx/blast_door_1.opus",
    };
    constexpr std::array<std::string_view, 1> led_power_up_files {
      "audio/sfx/led_power_up_0.opus",
    };
    constexpr std::array<std::string_view, 1> qte_alert_files {
      "audio/sfx/qte_alert_0.opus",
    };
    constexpr std::array<std::string_view, 1> qte_success_files {
      "audio/sfx/qte_success_0.opus",
    };
    constexpr std::array<std::string_view, 2> qte_fail_files {
      "audio/sfx/qte_fail_0.opus",
      "audio/sfx/qte_fail_1.opus",
    };
    constexpr std::array<std::string_view, 2> neon_burst_files {
      "audio/sfx/neon_burst_0.opus",
      "audio/sfx/neon_burst_1.opus",
    };
    constexpr std::array<std::string_view, 1> lure_beep_files {
      "audio/sfx/lure_beep_0.opus",
    };
    constexpr std::array<std::string_view, 1> battery_charge_files {
      "audio/sfx/battery_charge_0.opus",
    };
    constexpr std::array<std::string_view, 2> ui_glitch_files {
      "audio/sfx/ui_glitch_0.opus",
      "audio/sfx/ui_glitch_1.opus",
    };
  } // namespace

  std::span<const std::string_view> SoundCatalog::filesFor(SoundEnum sound) {
    switch(sound) {
      case SoundEnum::footstep:              return footstep_files;
      case SoundEnum::crawler_skitter:       return crawler_skitter_files;
      case SoundEnum::crawler_chitter:       return crawler_chitter_files;
      case SoundEnum::crumble:               return crumble_files;
      case SoundEnum::explosion:             return explosion_files;
      case SoundEnum::tracker_open:          return tracker_open_files;
      case SoundEnum::tracker_close:         return tracker_close_files;
      case SoundEnum::tracker_blip:          return tracker_blip_files;
      case SoundEnum::generator_tick:        return generator_tick_files;
      case SoundEnum::generator_online:      return generator_online_files;
      case SoundEnum::generator_breakdown:   return generator_breakdown_files;
      case SoundEnum::generator_request:     return generator_request_files;
      case SoundEnum::part_pickup:           return part_pickup_files;
      case SoundEnum::door_open:             return door_open_files;
      case SoundEnum::player_hurt:           return player_hurt_files;
      case SoundEnum::heartbeat:             return heartbeat_files;
      case SoundEnum::machine_work:          return machine_work_files;
      case SoundEnum::switch_click:          return switch_click_files;
      case SoundEnum::acid_sizzle:           return acid_sizzle_files;
      case SoundEnum::shepherd_call:         return shepherd_call_files;
      case SoundEnum::shepherd_step:         return shepherd_step_files;
      case SoundEnum::moth_flutter:          return moth_flutter_files;
      case SoundEnum::player_death:          return player_death_files;
      case SoundEnum::phase_start:           return phase_start_files;
      case SoundEnum::whisper:               return whisper_files;
      case SoundEnum::jaws:                  return jaws_files;
      case SoundEnum::electric_buzz:         return electric_buzz_files;
      case SoundEnum::reinforcement_whistle: return reinforcement_whistle_files;
      case SoundEnum::metal_clang:           return metal_clang_files;
      case SoundEnum::distant_scream:        return distant_scream_files;
      case SoundEnum::glass_shatter:         return glass_shatter_files;
      case SoundEnum::decoy_throw:           return decoy_throw_files;
      case SoundEnum::decoy_impact:          return decoy_impact_files;
      case SoundEnum::locker_door:           return locker_door_files;
      case SoundEnum::mob_burn:              return mob_burn_files;
      case SoundEnum::breathing:             return breathing_files;
      case SoundEnum::bloater_gurgle:        return bloater_gurgle_files;
      case SoundEnum::melter_slither:        return melter_slither_files;
      case SoundEnum::mimic_clank:           return mimic_clank_files;
      case SoundEnum::blast_door:            return blast_door_files;
      case SoundEnum::led_power_up:          return led_power_up_files;
      case SoundEnum::qte_alert:             return qte_alert_files;
      case SoundEnum::qte_success:           return qte_success_files;
      case SoundEnum::qte_fail:              return qte_fail_files;
      case SoundEnum::neon_burst:            return neon_burst_files;
      case SoundEnum::lure_beep:             return lure_beep_files;
      case SoundEnum::battery_charge:        return battery_charge_files;
      case SoundEnum::ui_glitch:             return ui_glitch_files;
    }

    return footstep_files;
  }

  std::string_view SoundCatalog::musicFor(MusicLayerEnum layer) {
    switch(layer) {
      case MusicLayerEnum::ambience: return "audio/music/ambience.opus";
      case MusicLayerEnum::tension:  return "audio/music/tension.opus";
      case MusicLayerEnum::title:    return "audio/music/title.opus";
    }

    return "";
  }
} // namespace wheel_of_fear
