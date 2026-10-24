#pragma once

#include "mods/world/RoomLedStrip.hpp"
#include "mods/world/RoomLightingPlanProps.hpp"
#include "mods/world/TileMapModel.hpp"
#include <cstddef>
#include <vector>

namespace wheel_of_fear {
  class RoomLightingPlanner final {
    public:
      RoomLightingPlanner()                            = delete;
      static constexpr int         maximum_columns     = 2;
      static constexpr int         maximum_rows        = 2;
      static constexpr float       target_cell_size    = 240;
      static constexpr float       reach_per_half_cell = 4;
      static constexpr float       minimum_radius      = 180;
      static RoomLightingPlanProps plan(
        const TileMapModel &map,
        std::size_t         room_index,
        const RoomLedStrip &strip
      );

    private:
      static std::vector<RoomFillLightProps> planFillLights(
        const TileMapModel &map,
        std::size_t         room_index,
        const RoomLedStrip &strip
      );

      static std::vector<RoomGlowTileProps> planGlowTiles(
        const TileMapModel &map,
        std::size_t         room_index,
        const RoomLedStrip &strip
      );

      static int cellCount(
        float extent,
        int   maximum
      );
  };
} // namespace wheel_of_fear
