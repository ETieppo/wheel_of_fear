#pragma once

#include "mods/world/RoomFillLightProps.hpp"
#include "mods/world/RoomGlowTileProps.hpp"
#include <vector>

namespace wheel_of_fear {
  struct RoomLightingPlanProps {
      std::vector<RoomFillLightProps> fill_lights;
      std::vector<RoomGlowTileProps>  glow_tiles;
  };
} // namespace wheel_of_fear
