#pragma once

#include "core/assets/SpriteStripProps.hpp"

namespace wheel_of_fear {
  class SpriteCatalog final {
    public:
      SpriteCatalog() = delete;

      static constexpr SpriteStripProps barrel {
        "sprites/barrel.png", 1, 96, 96, 8
      };
      static constexpr SpriteStripProps crate {
        "sprites/crate.png", 1, 112, 112, 8
      };
      static constexpr SpriteStripProps decoy_can {
        "sprites/decoy_can.png", 1, 48, 48, 8
      };
      static constexpr SpriteStripProps door_frame {
        "sprites/door_frame.png", 1, 288, 192, 8
      };
      static constexpr SpriteStripProps door_left {
        "sprites/door_left.png", 1, 128, 160, 8
      };
      static constexpr SpriteStripProps door_right {
        "sprites/door_right.png", 1, 128, 160, 8
      };
      static constexpr SpriteStripProps eye_monster {
        "sprites/eye_monster.png", 8, 192, 192, 8
      };
      static constexpr SpriteStripProps generator {
        "sprites/generator.png", 1, 208, 192, 8
      };
      static constexpr SpriteStripProps generator_gear {
        "sprites/generator_gear.png", 1, 104, 104, 8
      };
      static constexpr SpriteStripProps icon_battery {
        "sprites/icon_battery.png", 1, 80, 80, 8
      };
      static constexpr SpriteStripProps icon_fuse {
        "sprites/icon_fuse.png", 1, 80, 80, 8
      };
      static constexpr SpriteStripProps icon_gear {
        "sprites/icon_gear.png", 1, 80, 80, 8
      };
      static constexpr SpriteStripProps icon_valve {
        "sprites/icon_valve.png", 1, 80, 80, 8
      };
      static constexpr SpriteStripProps junction_box {
        "sprites/junction_box.png", 1, 96, 96, 8
      };
      static constexpr SpriteStripProps locker_closed {
        "sprites/locker_closed.png", 1, 96, 160, 8
      };
      static constexpr SpriteStripProps locker_open {
        "sprites/locker_open.png", 1, 96, 160, 8
      };
      static constexpr SpriteStripProps machine {
        "sprites/machine.png", 1, 160, 176, 8
      };
      static constexpr SpriteStripProps neon_sign_0 {
        "sprites/neon_sign_0.png", 1, 128, 80, 8
      };
      static constexpr SpriteStripProps neon_sign_1 {
        "sprites/neon_sign_1.png", 1, 128, 80, 8
      };
      static constexpr SpriteStripProps neon_sign_2 {
        "sprites/neon_sign_2.png", 1, 128, 80, 8
      };
      static constexpr SpriteStripProps neon_tube_core {
        "sprites/neon_tube_core.png", 1, 144, 48, 8
      };
      static constexpr SpriteStripProps neon_tube_housing {
        "sprites/neon_tube_housing.png", 1, 144, 48, 8
      };
      static constexpr SpriteStripProps particle_circle {
        "sprites/particle_circle.png", 1, 128, 128, 8
      };
      static constexpr SpriteStripProps particle_dirt {
        "sprites/particle_dirt.png", 3, 128, 128, 8
      };
      static constexpr SpriteStripProps particle_flare {
        "sprites/particle_flare.png", 1, 128, 128, 8
      };
      static constexpr SpriteStripProps particle_light {
        "sprites/particle_light.png", 3, 128, 128, 8
      };
      static constexpr SpriteStripProps particle_magic {
        "sprites/particle_magic.png", 5, 128, 128, 8
      };
      static constexpr SpriteStripProps particle_scorch {
        "sprites/particle_scorch.png", 3, 128, 128, 8
      };
      static constexpr SpriteStripProps particle_smoke {
        "sprites/particle_smoke.png", 5, 128, 128, 8
      };
      static constexpr SpriteStripProps particle_spark {
        "sprites/particle_spark.png", 7, 128, 128, 8
      };
      static constexpr SpriteStripProps particle_twirl {
        "sprites/particle_twirl.png", 1, 128, 128, 8
      };
      static constexpr SpriteStripProps pipes {
        "sprites/pipes.png", 1, 128, 128, 8
      };
      static constexpr SpriteStripProps player_feet {
        "sprites/player_feet.png", 20, 208, 162, 8
      };
      static constexpr SpriteStripProps player_hug {
        "sprites/player_hug.png", 15, 208, 162, 8
      };
      static constexpr SpriteStripProps player_idle {
        "sprites/player_idle.png", 20, 208, 162, 8
      };
      static constexpr SpriteStripProps player_move {
        "sprites/player_move.png", 20, 208, 162, 8
      };
      static constexpr SpriteStripProps teeth_monster {
        "sprites/teeth_monster.png", 8, 192, 192, 8
      };
      static constexpr SpriteStripProps vent {
        "sprites/vent.png", 1, 112, 80, 8
      };
      static constexpr SpriteStripProps zombie_attack {
        "sprites/zombie_attack.png", 9, 176, 188, 8
      };
      static constexpr SpriteStripProps zombie_move {
        "sprites/zombie_move.png", 17, 176, 188, 8
      };

      static constexpr SpriteStripProps generated_blast_door_frame {
        "generated/sprites/blast_door_frame.png", 1, 448, 128, 8
      };
      static constexpr SpriteStripProps generated_blast_door_panel {
        "generated/sprites/blast_door_panel.png", 1, 200, 112, 8
      };
      static constexpr SpriteStripProps generated_bloater_drift {
        "generated/sprites/bloater_drift.png", 8, 208, 208, 8
      };
      static constexpr SpriteStripProps generated_bloater_drift_glow {
        "generated/sprites/bloater_drift_glow.png", 8, 104, 104, 4
      };
      static constexpr SpriteStripProps generated_crawler_walk {
        "generated/sprites/crawler_walk.png", 8, 208, 208, 8
      };
      static constexpr SpriteStripProps generated_crawler_walk_glow {
        "generated/sprites/crawler_walk_glow.png", 8, 104, 104, 4
      };
      static constexpr SpriteStripProps generated_exit_frame {
        "generated/sprites/exit_frame.png", 1, 480, 208, 8
      };
      static constexpr SpriteStripProps generated_exit_frame_glow {
        "generated/sprites/exit_frame_glow.png", 1, 240, 104, 4
      };
      static constexpr SpriteStripProps generated_fabricator {
        "generated/sprites/fabricator.png", 1, 160, 192, 8
      };
      static constexpr SpriteStripProps generated_fabricator_glow {
        "generated/sprites/fabricator_glow.png", 1, 80, 96, 4
      };
      static constexpr SpriteStripProps generated_generator_base {
        "generated/sprites/generator_base.png", 1, 320, 320, 8
      };
      static constexpr SpriteStripProps generated_generator_base_glow {
        "generated/sprites/generator_base_glow.png", 1, 160, 160, 4
      };
      static constexpr SpriteStripProps generated_generator_rotor {
        "generated/sprites/generator_rotor.png", 1, 176, 176, 8
      };
      static constexpr SpriteStripProps generated_generator_rotor_glow {
        "generated/sprites/generator_rotor_glow.png", 1, 88, 88, 4
      };
      static constexpr SpriteStripProps generated_icon_grenade {
        "generated/sprites/icon_grenade.png", 1, 80, 80, 8
      };
      static constexpr SpriteStripProps generated_icon_lure {
        "generated/sprites/icon_lure.png", 1, 80, 80, 8
      };
      static constexpr SpriteStripProps generated_lure_beacon {
        "generated/sprites/lure_beacon.png", 1, 64, 64, 8
      };
      static constexpr SpriteStripProps generated_lure_beacon_glow {
        "generated/sprites/lure_beacon_glow.png", 1, 32, 32, 4
      };
      static constexpr SpriteStripProps generated_melter_crawl {
        "generated/sprites/melter_crawl.png", 8, 208, 208, 8
      };
      static constexpr SpriteStripProps generated_melter_crawl_glow {
        "generated/sprites/melter_crawl_glow.png", 8, 104, 104, 4
      };
      static constexpr SpriteStripProps generated_mimic_hunt {
        "generated/sprites/mimic_hunt.png", 6, 224, 224, 8
      };
      static constexpr SpriteStripProps generated_mimic_hunt_glow {
        "generated/sprites/mimic_hunt_glow.png", 6, 112, 112, 4
      };
      static constexpr SpriteStripProps generated_moth_flutter {
        "generated/sprites/moth_flutter.png", 4, 112, 112, 8
      };
      static constexpr SpriteStripProps generated_moth_flutter_glow {
        "generated/sprites/moth_flutter_glow.png", 4, 56, 56, 4
      };
      static constexpr SpriteStripProps generated_neon_grenade {
        "generated/sprites/neon_grenade.png", 1, 64, 64, 8
      };
      static constexpr SpriteStripProps generated_neon_grenade_glow {
        "generated/sprites/neon_grenade_glow.png", 1, 32, 32, 4
      };
      static constexpr SpriteStripProps generated_player_hug {
        "generated/sprites/player_hug.png", 8, 192, 192, 8
      };
      static constexpr SpriteStripProps generated_player_hug_glow {
        "generated/sprites/player_hug_glow.png", 8, 96, 96, 4
      };
      static constexpr SpriteStripProps generated_player_idle {
        "generated/sprites/player_idle.png", 8, 192, 192, 8
      };
      static constexpr SpriteStripProps generated_player_idle_glow {
        "generated/sprites/player_idle_glow.png", 8, 96, 96, 4
      };
      static constexpr SpriteStripProps generated_player_repair {
        "generated/sprites/player_repair.png", 6, 192, 192, 8
      };
      static constexpr SpriteStripProps generated_player_repair_glow {
        "generated/sprites/player_repair_glow.png", 6, 96, 96, 4
      };
      static constexpr SpriteStripProps generated_player_walk {
        "generated/sprites/player_walk.png", 12, 192, 192, 8
      };
      static constexpr SpriteStripProps generated_player_walk_glow {
        "generated/sprites/player_walk_glow.png", 12, 96, 96, 4
      };
      static constexpr SpriteStripProps generated_server_rack {
        "generated/sprites/server_rack.png", 1, 128, 128, 8
      };
      static constexpr SpriteStripProps generated_server_rack_glow {
        "generated/sprites/server_rack_glow.png", 1, 64, 64, 4
      };
      static constexpr SpriteStripProps generated_shade_drift {
        "generated/sprites/shade_drift.png", 8, 208, 208, 8
      };
      static constexpr SpriteStripProps generated_shade_drift_glow {
        "generated/sprites/shade_drift_glow.png", 8, 104, 104, 4
      };
      static constexpr SpriteStripProps generated_shepherd_walk {
        "generated/sprites/shepherd_walk.png", 8, 224, 224, 8
      };
      static constexpr SpriteStripProps generated_shepherd_walk_glow {
        "generated/sprites/shepherd_walk_glow.png", 8, 112, 112, 4
      };
      static constexpr SpriteStripProps generated_terminal {
        "generated/sprites/terminal.png", 1, 128, 128, 8
      };
      static constexpr SpriteStripProps generated_terminal_glow {
        "generated/sprites/terminal_glow.png", 1, 64, 64, 4
      };
  };
} // namespace wheel_of_fear
