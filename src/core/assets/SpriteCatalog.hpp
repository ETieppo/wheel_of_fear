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
      static constexpr float            tree_pixels_per_unit = 3;
      static constexpr SpriteStripProps tree_00 {
        .path            = "sprites/trees/tree_00.png",
        .frame_count     = 1,
        .frame_width     = 192,
        .frame_height    = 192,
        .pixels_per_unit = tree_pixels_per_unit,
      };
      static constexpr SpriteStripProps tree_01 {
        .path            = "sprites/trees/tree_01.png",
        .frame_count     = 1,
        .frame_width     = 192,
        .frame_height    = 192,
        .pixels_per_unit = tree_pixels_per_unit,
      };
      static constexpr SpriteStripProps tree_02 {
        .path            = "sprites/trees/tree_02.png",
        .frame_count     = 1,
        .frame_width     = 192,
        .frame_height    = 192,
        .pixels_per_unit = tree_pixels_per_unit,
      };
      static constexpr SpriteStripProps tree_03 {
        .path            = "sprites/trees/tree_03.png",
        .frame_count     = 1,
        .frame_width     = 192,
        .frame_height    = 192,
        .pixels_per_unit = tree_pixels_per_unit,
      };
      static constexpr SpriteStripProps tree_04 {
        .path            = "sprites/trees/tree_04.png",
        .frame_count     = 1,
        .frame_width     = 192,
        .frame_height    = 192,
        .pixels_per_unit = tree_pixels_per_unit,
      };
      static constexpr SpriteStripProps tree_05 {
        .path            = "sprites/trees/tree_05.png",
        .frame_count     = 1,
        .frame_width     = 192,
        .frame_height    = 192,
        .pixels_per_unit = tree_pixels_per_unit,
      };
      static constexpr SpriteStripProps tree_06 {
        .path            = "sprites/trees/tree_06.png",
        .frame_count     = 1,
        .frame_width     = 192,
        .frame_height    = 192,
        .pixels_per_unit = tree_pixels_per_unit,
      };
      static constexpr SpriteStripProps tree_07 {
        .path            = "sprites/trees/tree_07.png",
        .frame_count     = 1,
        .frame_width     = 192,
        .frame_height    = 192,
        .pixels_per_unit = tree_pixels_per_unit,
      };
      static constexpr SpriteStripProps tree_08 {
        .path            = "sprites/trees/tree_08.png",
        .frame_count     = 1,
        .frame_width     = 192,
        .frame_height    = 192,
        .pixels_per_unit = tree_pixels_per_unit,
      };
      static constexpr SpriteStripProps tree_09 {
        .path            = "sprites/trees/tree_09.png",
        .frame_count     = 1,
        .frame_width     = 192,
        .frame_height    = 192,
        .pixels_per_unit = tree_pixels_per_unit,
      };
      static constexpr SpriteStripProps tree_10 {
        .path            = "sprites/trees/tree_10.png",
        .frame_count     = 1,
        .frame_width     = 192,
        .frame_height    = 192,
        .pixels_per_unit = tree_pixels_per_unit,
      };
      static constexpr SpriteStripProps tree_11 {
        .path            = "sprites/trees/tree_11.png",
        .frame_count     = 1,
        .frame_width     = 192,
        .frame_height    = 192,
        .pixels_per_unit = tree_pixels_per_unit,
      };
      static constexpr SpriteStripProps tree_12 {
        .path            = "sprites/trees/tree_12.png",
        .frame_count     = 1,
        .frame_width     = 192,
        .frame_height    = 192,
        .pixels_per_unit = tree_pixels_per_unit,
      };
      static constexpr SpriteStripProps letter {
        .path            = "sprites/letter.png",
        .frame_count     = 1,
        .frame_width     = 96,
        .frame_height    = 120,
        .pixels_per_unit = 6,
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

      static constexpr SpriteStripProps blast_door_frame {
        .path            = "sprites/blast_door_frame.png",
        .frame_count     = 1,
        .frame_width     = 448,
        .frame_height    = 128,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps blast_door_panel {
        .path            = "sprites/blast_door_panel.png",
        .frame_count     = 1,
        .frame_width     = 200,
        .frame_height    = 112,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps bloater_drift {
        .path            = "sprites/bloater_drift.png",
        .frame_count     = 10,
        .frame_width     = 208,
        .frame_height    = 208,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps crawler_walk {
        .path            = "sprites/crawler_walk.png",
        .frame_count     = 4,
        .frame_width     = 208,
        .frame_height    = 208,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps exit_frame {
        .path            = "sprites/exit_frame.png",
        .frame_count     = 1,
        .frame_width     = 480,
        .frame_height    = 208,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps fabricator {
        .path            = "sprites/fabricator.png",
        .frame_count     = 1,
        .frame_width     = 160,
        .frame_height    = 192,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generator_base {
        .path            = "sprites/generator_base.png",
        .frame_count     = 1,
        .frame_width     = 320,
        .frame_height    = 320,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps generator_rotor {
        .path            = "sprites/generator_rotor.png",
        .frame_count     = 1,
        .frame_width     = 176,
        .frame_height    = 176,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps icon_grenade {
        .path            = "sprites/icon_grenade.png",
        .frame_count     = 1,
        .frame_width     = 80,
        .frame_height    = 80,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps icon_lure {
        .path            = "sprites/icon_lure.png",
        .frame_count     = 1,
        .frame_width     = 80,
        .frame_height    = 80,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps lure_beacon {
        .path            = "sprites/lure_beacon.png",
        .frame_count     = 1,
        .frame_width     = 64,
        .frame_height    = 64,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps melter_crawl {
        .path            = "sprites/melter_crawl.png",
        .frame_count     = 4,
        .frame_width     = 208,
        .frame_height    = 208,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps mimic_hunt {
        .path            = "sprites/mimic_hunt.png",
        .frame_count     = 1,
        .frame_width     = 224,
        .frame_height    = 224,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps moth_flutter {
        .path            = "sprites/moth_flutter.png",
        .frame_count     = 3,
        .frame_width     = 112,
        .frame_height    = 112,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps neon_grenade {
        .path            = "sprites/neon_grenade.png",
        .frame_count     = 1,
        .frame_width     = 64,
        .frame_height    = 64,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps server_rack {
        .path            = "sprites/server_rack.png",
        .frame_count     = 1,
        .frame_width     = 128,
        .frame_height    = 128,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps shade_drift {
        .path            = "sprites/shade_drift.png",
        .frame_count     = 15,
        .frame_width     = 208,
        .frame_height    = 208,
        .pixels_per_unit = 8,
      };
      static constexpr SpriteStripProps terminal {
        .path            = "sprites/terminal.png",
        .frame_count     = 1,
        .frame_width     = 128,
        .frame_height    = 128,
        .pixels_per_unit = 8,
      };
  };
} // namespace wheel_of_fear
