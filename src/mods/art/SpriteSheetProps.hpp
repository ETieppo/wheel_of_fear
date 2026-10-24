#pragma once

#include <array>
#include <vector>

#include "core/items/enums/ItemEnum.hpp"
#include "core/render/AtlasRegionProps.hpp"

namespace wheel_of_fear {
  using SpriteFrames = std::vector<AtlasRegionProps>;

  struct SpriteSheetProps {
      SpriteFrames                             player_walk;
      SpriteFrames                             player_walk_glow;
      SpriteFrames                             player_idle;
      SpriteFrames                             player_idle_glow;
      SpriteFrames                             player_hug;
      SpriteFrames                             player_hug_glow;
      SpriteFrames                             player_repair;
      SpriteFrames                             player_repair_glow;
      SpriteFrames                             crawler_walk;
      SpriteFrames                             crawler_walk_glow;
      SpriteFrames                             bloater_drift;
      SpriteFrames                             bloater_drift_glow;
      SpriteFrames                             melter_crawl;
      SpriteFrames                             melter_crawl_glow;
      SpriteFrames                             shepherd_walk;
      SpriteFrames                             shepherd_walk_glow;
      SpriteFrames                             shade_drift;
      SpriteFrames                             shade_drift_glow;
      SpriteFrames                             mimic_hunt;
      SpriteFrames                             mimic_hunt_glow;
      SpriteFrames                             moth_flutter;
      SpriteFrames                             moth_flutter_glow;
      AtlasRegionProps                         generator_base;
      AtlasRegionProps                         generator_glow;
      AtlasRegionProps                         generator_rotor;
      AtlasRegionProps                         generator_rotor_glow;
      AtlasRegionProps                         fabricator;
      AtlasRegionProps                         fabricator_glow;
      AtlasRegionProps                         server_rack;
      AtlasRegionProps                         server_rack_glow;
      AtlasRegionProps                         terminal;
      AtlasRegionProps                         terminal_glow;
      AtlasRegionProps                         exit_frame;
      AtlasRegionProps                         exit_frame_glow;
      AtlasRegionProps                         blast_door_frame;
      AtlasRegionProps                         blast_door_panel;
      AtlasRegionProps                         lure_beacon;
      AtlasRegionProps                         neon_grenade;
      SpriteFrames                             spark;
      SpriteFrames                             light;
      AtlasRegionProps                         flare;
      SpriteFrames                             smoke;
      SpriteFrames                             scorch;
      SpriteFrames                             dirt;
      AtlasRegionProps                         soft_circle;
      AtlasRegionProps                         twirl;
      AtlasRegionProps                         barrel;
      AtlasRegionProps                         crate;
      AtlasRegionProps                         pipes;
      AtlasRegionProps                         junction_box;
      AtlasRegionProps                         locker_closed;
      AtlasRegionProps                         locker_open;
      AtlasRegionProps                         vent;
      std::array<AtlasRegionProps, 3>          neon_signs {};
      std::array<AtlasRegionProps, item_count> item_icons {};

      [[nodiscard]]
      const AtlasRegionProps &itemIcon(
        ItemEnum item
      ) const {
        return item_icons[indexOf(item)];
      }
  };
} // namespace wheel_of_fear
