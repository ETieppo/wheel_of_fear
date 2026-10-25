#pragma once

#include "core/assets/SpriteStripProps.hpp"

namespace wheel_of_fear {
  class SpriteCatalog final {
    public:
      SpriteCatalog() = delete;

      static constexpr SpriteStripProps barrel {
        .path            = "sprites/barrel.png",
        .frame_count     = 1,
        .frame_width     = 96,
        .frame_height    = 96,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps crate {
        .path            = "sprites/crate.png",
        .frame_count     = 1,
        .frame_width     = 112,
        .frame_height    = 112,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps decoy_can {
        .path            = "sprites/decoy_can.png",
        .frame_count     = 1,
        .frame_width     = 48,
        .frame_height    = 48,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps door_frame {
        .path            = "sprites/door_frame.png",
        .frame_count     = 1,
        .frame_width     = 288,
        .frame_height    = 192,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps door_left {
        .path            = "sprites/door_left.png",
        .frame_count     = 1,
        .frame_width     = 128,
        .frame_height    = 160,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps door_right {
        .path            = "sprites/door_right.png",
        .frame_count     = 1,
        .frame_width     = 128,
        .frame_height    = 160,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps eye_monster {
        .path            = "sprites/eye_monster.png",
        .frame_count     = 8,
        .frame_width     = 192,
        .frame_height    = 192,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generator {
        .path            = "sprites/generator.png",
        .frame_count     = 1,
        .frame_width     = 208,
        .frame_height    = 192,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generator_gear {
        .path            = "sprites/generator_gear.png",
        .frame_count     = 1,
        .frame_width     = 104,
        .frame_height    = 104,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps icon_battery {
        .path            = "sprites/icon_battery.png",
        .frame_count     = 1,
        .frame_width     = 80,
        .frame_height    = 80,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps icon_fuse {
        .path            = "sprites/icon_fuse.png",
        .frame_count     = 1,
        .frame_width     = 80,
        .frame_height    = 80,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps icon_gear {
        .path            = "sprites/icon_gear.png",
        .frame_count     = 1,
        .frame_width     = 80,
        .frame_height    = 80,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps icon_valve {
        .path            = "sprites/icon_valve.png",
        .frame_count     = 1,
        .frame_width     = 80,
        .frame_height    = 80,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps junction_box {
        .path            = "sprites/junction_box.png",
        .frame_count     = 1,
        .frame_width     = 96,
        .frame_height    = 96,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps locker_closed {
        .path            = "sprites/locker_closed.png",
        .frame_count     = 1,
        .frame_width     = 96,
        .frame_height    = 160,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps locker_open {
        .path            = "sprites/locker_open.png",
        .frame_count     = 1,
        .frame_width     = 96,
        .frame_height    = 160,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps machine {
        .path            = "sprites/machine.png",
        .frame_count     = 1,
        .frame_width     = 160,
        .frame_height    = 176,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps neon_tube_core {
        .path            = "sprites/neon_tube_core.png",
        .frame_count     = 1,
        .frame_width     = 144,
        .frame_height    = 48,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps neon_tube_housing {
        .path            = "sprites/neon_tube_housing.png",
        .frame_count     = 1,
        .frame_width     = 144,
        .frame_height    = 48,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps particle_circle {
        .path            = "sprites/particle_circle.png",
        .frame_count     = 1,
        .frame_width     = 128,
        .frame_height    = 128,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps particle_dirt {
        .path            = "sprites/particle_dirt.png",
        .frame_count     = 3,
        .frame_width     = 128,
        .frame_height    = 128,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps particle_flare {
        .path            = "sprites/particle_flare.png",
        .frame_count     = 1,
        .frame_width     = 128,
        .frame_height    = 128,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps particle_light {
        .path            = "sprites/particle_light.png",
        .frame_count     = 3,
        .frame_width     = 128,
        .frame_height    = 128,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps particle_magic {
        .path            = "sprites/particle_magic.png",
        .frame_count     = 5,
        .frame_width     = 128,
        .frame_height    = 128,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps particle_scorch {
        .path            = "sprites/particle_scorch.png",
        .frame_count     = 3,
        .frame_width     = 128,
        .frame_height    = 128,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps particle_smoke {
        .path            = "sprites/particle_smoke.png",
        .frame_count     = 5,
        .frame_width     = 128,
        .frame_height    = 128,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps particle_spark {
        .path            = "sprites/particle_spark.png",
        .frame_count     = 7,
        .frame_width     = 128,
        .frame_height    = 128,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps particle_twirl {
        .path            = "sprites/particle_twirl.png",
        .frame_count     = 1,
        .frame_width     = 128,
        .frame_height    = 128,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps pipes {
        .path            = "sprites/pipes.png",
        .frame_count     = 1,
        .frame_width     = 128,
        .frame_height    = 128,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps player_feet {
        .path            = "sprites/player_feet.png",
        .frame_count     = 20,
        .frame_width     = 208,
        .frame_height    = 162,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps player_hug {
        .path            = "sprites/player_hug.png",
        .frame_count     = 15,
        .frame_width     = 208,
        .frame_height    = 162,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps player_idle {
        .path            = "sprites/player_idle.png",
        .frame_count     = 20,
        .frame_width     = 208,
        .frame_height    = 162,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps player_move {
        .path            = "sprites/player_move.png",
        .frame_count     = 20,
        .frame_width     = 208,
        .frame_height    = 162,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps teeth_monster {
        .path            = "sprites/teeth_monster.png",
        .frame_count     = 8,
        .frame_width     = 192,
        .frame_height    = 192,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps vent {
        .path            = "sprites/vent.png",
        .frame_count     = 1,
        .frame_width     = 112,
        .frame_height    = 80,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps zombie_attack {
        .path            = "sprites/zombie_attack.png",
        .frame_count     = 9,
        .frame_width     = 176,
        .frame_height    = 188,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps zombie_move {
        .path            = "sprites/zombie_move.png",
        .frame_count     = 17,
        .frame_width     = 176,
        .frame_height    = 188,
        .pixels_per_unit = 8,
      };

      static constexpr SpriteStripProps generated_blast_door_frame {
        .path            = "generated/sprites/blast_door_frame.png",
        .frame_count     = 1,
        .frame_width     = 448,
        .frame_height    = 128,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_blast_door_panel {
        .path            = "generated/sprites/blast_door_panel.png",
        .frame_count     = 1,
        .frame_width     = 200,
        .frame_height    = 112,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_bloater_drift {
        .path            = "generated/sprites/bloater_drift.png",
        .frame_count     = 8,
        .frame_width     = 208,
        .frame_height    = 208,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_bloater_drift_glow {
        .path            = "generated/sprites/bloater_drift_glow.png",
        .frame_count     = 8,
        .frame_width     = 104,
        .frame_height    = 104,
        .pixels_per_unit = 4,
      };
      static constexpr SpriteStripProps generated_crawler_walk {
        .path            = "generated/sprites/crawler_walk.png",
        .frame_count     = 8,
        .frame_width     = 208,
        .frame_height    = 208,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_crawler_walk_glow {
        .path            = "generated/sprites/crawler_walk_glow.png",
        .frame_count     = 8,
        .frame_width     = 104,
        .frame_height    = 104,
        .pixels_per_unit = 4,
      };
      static constexpr SpriteStripProps generated_exit_frame {
        .path            = "generated/sprites/exit_frame.png",
        .frame_count     = 1,
        .frame_width     = 480,
        .frame_height    = 208,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_exit_frame_glow {
        .path            = "generated/sprites/exit_frame_glow.png",
        .frame_count     = 1,
        .frame_width     = 240,
        .frame_height    = 104,
        .pixels_per_unit = 4,
      };
      static constexpr SpriteStripProps generated_fabricator {
        .path            = "generated/sprites/fabricator.png",
        .frame_count     = 1,
        .frame_width     = 160,
        .frame_height    = 192,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_fabricator_glow {
        .path            = "generated/sprites/fabricator_glow.png",
        .frame_count     = 1,
        .frame_width     = 80,
        .frame_height    = 96,
        .pixels_per_unit = 4,
      };
      static constexpr SpriteStripProps generated_generator_base {
        .path            = "generated/sprites/generator_base.png",
        .frame_count     = 1,
        .frame_width     = 320,
        .frame_height    = 320,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_generator_base_glow {
        .path            = "generated/sprites/generator_base_glow.png",
        .frame_count     = 1,
        .frame_width     = 160,
        .frame_height    = 160,
        .pixels_per_unit = 4,
      };
      static constexpr SpriteStripProps generated_generator_rotor {
        .path            = "generated/sprites/generator_rotor.png",
        .frame_count     = 1,
        .frame_width     = 176,
        .frame_height    = 176,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_generator_rotor_glow {
        .path            = "generated/sprites/generator_rotor_glow.png",
        .frame_count     = 1,
        .frame_width     = 88,
        .frame_height    = 88,
        .pixels_per_unit = 4,
      };
      static constexpr SpriteStripProps generated_icon_grenade {
        .path            = "generated/sprites/icon_grenade.png",
        .frame_count     = 1,
        .frame_width     = 80,
        .frame_height    = 80,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_icon_lure {
        .path            = "generated/sprites/icon_lure.png",
        .frame_count     = 1,
        .frame_width     = 80,
        .frame_height    = 80,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_lure_beacon {
        .path            = "generated/sprites/lure_beacon.png",
        .frame_count     = 1,
        .frame_width     = 64,
        .frame_height    = 64,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_lure_beacon_glow {
        .path            = "generated/sprites/lure_beacon_glow.png",
        .frame_count     = 1,
        .frame_width     = 32,
        .frame_height    = 32,
        .pixels_per_unit = 4,
      };
      static constexpr SpriteStripProps generated_melter_crawl {
        .path            = "generated/sprites/melter_crawl.png",
        .frame_count     = 8,
        .frame_width     = 208,
        .frame_height    = 208,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_melter_crawl_glow {
        .path            = "generated/sprites/melter_crawl_glow.png",
        .frame_count     = 8,
        .frame_width     = 104,
        .frame_height    = 104,
        .pixels_per_unit = 4,
      };
      static constexpr SpriteStripProps generated_mimic_hunt {
        .path            = "generated/sprites/mimic_hunt.png",
        .frame_count     = 6,
        .frame_width     = 224,
        .frame_height    = 224,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_mimic_hunt_glow {
        .path            = "generated/sprites/mimic_hunt_glow.png",
        .frame_count     = 6,
        .frame_width     = 112,
        .frame_height    = 112,
        .pixels_per_unit = 4,
      };
      static constexpr SpriteStripProps generated_moth_flutter {
        .path            = "generated/sprites/moth_flutter.png",
        .frame_count     = 4,
        .frame_width     = 112,
        .frame_height    = 112,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_moth_flutter_glow {
        .path            = "generated/sprites/moth_flutter_glow.png",
        .frame_count     = 4,
        .frame_width     = 56,
        .frame_height    = 56,
        .pixels_per_unit = 4,
      };
      static constexpr SpriteStripProps generated_neon_grenade {
        .path            = "generated/sprites/neon_grenade.png",
        .frame_count     = 1,
        .frame_width     = 64,
        .frame_height    = 64,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_neon_grenade_glow {
        .path            = "generated/sprites/neon_grenade_glow.png",
        .frame_count     = 1,
        .frame_width     = 32,
        .frame_height    = 32,
        .pixels_per_unit = 4,
      };
      static constexpr SpriteStripProps generated_player_hug {
        .path            = "generated/sprites/player_hug.png",
        .frame_count     = 8,
        .frame_width     = 192,
        .frame_height    = 192,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_player_hug_glow {
        .path            = "generated/sprites/player_hug_glow.png",
        .frame_count     = 8,
        .frame_width     = 96,
        .frame_height    = 96,
        .pixels_per_unit = 4,
      };
      static constexpr SpriteStripProps generated_player_idle {
        .path            = "generated/sprites/player_idle.png",
        .frame_count     = 8,
        .frame_width     = 192,
        .frame_height    = 192,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_player_idle_glow {
        .path            = "generated/sprites/player_idle_glow.png",
        .frame_count     = 8,
        .frame_width     = 96,
        .frame_height    = 96,
        .pixels_per_unit = 4,
      };
      static constexpr SpriteStripProps generated_player_repair {
        .path            = "generated/sprites/player_repair.png",
        .frame_count     = 6,
        .frame_width     = 192,
        .frame_height    = 192,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_player_repair_glow {
        .path            = "generated/sprites/player_repair_glow.png",
        .frame_count     = 6,
        .frame_width     = 96,
        .frame_height    = 96,
        .pixels_per_unit = 4,
      };
      static constexpr SpriteStripProps generated_player_walk {
        .path            = "generated/sprites/player_walk.png",
        .frame_count     = 12,
        .frame_width     = 192,
        .frame_height    = 192,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_player_walk_glow {
        .path            = "generated/sprites/player_walk_glow.png",
        .frame_count     = 12,
        .frame_width     = 96,
        .frame_height    = 96,
        .pixels_per_unit = 4,
      };
      static constexpr SpriteStripProps generated_server_rack {
        .path            = "generated/sprites/server_rack.png",
        .frame_count     = 1,
        .frame_width     = 128,
        .frame_height    = 128,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_server_rack_glow {
        .path            = "generated/sprites/server_rack_glow.png",
        .frame_count     = 1,
        .frame_width     = 64,
        .frame_height    = 64,
        .pixels_per_unit = 4,
      };
      static constexpr SpriteStripProps generated_shade_drift {
        .path            = "generated/sprites/shade_drift.png",
        .frame_count     = 8,
        .frame_width     = 208,
        .frame_height    = 208,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_shade_drift_glow {
        .path            = "generated/sprites/shade_drift_glow.png",
        .frame_count     = 8,
        .frame_width     = 104,
        .frame_height    = 104,
        .pixels_per_unit = 4,
      };
      static constexpr SpriteStripProps generated_shepherd_walk {
        .path            = "generated/sprites/shepherd_walk.png",
        .frame_count     = 8,
        .frame_width     = 224,
        .frame_height    = 224,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_shepherd_walk_glow {
        .path            = "generated/sprites/shepherd_walk_glow.png",
        .frame_count     = 8,
        .frame_width     = 112,
        .frame_height    = 112,
        .pixels_per_unit = 4,
      };
      static constexpr SpriteStripProps generated_terminal {
        .path            = "generated/sprites/terminal.png",
        .frame_count     = 1,
        .frame_width     = 128,
        .frame_height    = 128,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generated_terminal_glow {
        .path            = "generated/sprites/terminal_glow.png",
        .frame_count     = 1,
        .frame_width     = 64,
        .frame_height    = 64,
        .pixels_per_unit = 4,
      };
  };
} // namespace wheel_of_fear
