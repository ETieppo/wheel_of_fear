#pragma once

#include "mods/world/RoomProps.hpp"
#include "mods/world/TileMapModel.hpp"

namespace wheel_of_fear {
  class TileMapConnectivityAnalyzer final {
    public:
      TileMapConnectivityAnalyzer() = delete;

      static bool isRoomGraphConnected(const TileMapModel &map);

      static bool isRoomOpenAreaConnected(
        const TileMapModel &map,
        const RoomProps    &room
      );

      static bool isEveryFloorConnected(const TileMapModel &map);
  };
} // namespace wheel_of_fear
