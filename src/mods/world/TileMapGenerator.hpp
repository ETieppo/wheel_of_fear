#pragma once

#include <cstddef>
#include <optional>

#include "core/math/RandomGenerator.hpp"
#include "mods/world/RoomProps.hpp"
#include "mods/world/TileAreaProps.hpp"
#include "mods/world/TileMapModel.hpp"

namespace wheel_of_fear {
  class TileMapGenerator {
    public:
      static constexpr int room_cell_width  = 30;
      static constexpr int room_cell_height = 20;

      TileMapGenerator(
        TileMapModel    &map,
        RandomGenerator &random
      )
          : map_(map)
          , random_(random) {
      }

      void generate(
        int grid_columns,
        int grid_rows
      );

    private:
      static constexpr int   doorway_corner_margin = 4;
      static constexpr float extra_doorway_chance  = 0.3F;
      static constexpr int   clearing_half_width   = 5;
      static constexpr int   clearing_half_height  = 4;

      static bool isInClearing(
        const RoomProps &room,
        int              x,
        int              y
      );

      [[nodiscard]]
      bool isInsideFurnishableArea(
        std::size_t room_index,
        int         x,
        int         y
      ) const;
      [[nodiscard]]
      std::optional<TileCoordinateProps> firstFloorTile(
        std::size_t room_index
      ) const;

      void carveRooms();
      void shapeRooms();
      void placeDoorways();
      void openSpanningDoorways();
      void furnishRoom(
        std::size_t room_index
      );
      void furnishHall(
        std::size_t room_index
      );
      void furnishOffices(
        std::size_t room_index
      );
      void furnishServerFarm(
        std::size_t room_index
      );
      void furnishStorage(
        std::size_t room_index
      );
      void furnishPumpStation(
        std::size_t room_index
      );
      bool placeWallBlock(
        std::size_t          room_index,
        const TileAreaProps &block
      );
      void sealUnreachableFloor(
        std::size_t room_index
      );
      [[nodiscard]]
      int randomInteriorX(
        const RoomProps &room
      ) const;
      [[nodiscard]]
      int randomInteriorY(
        const RoomProps &room
      ) const;

      TileMapModel    &map_;
      RandomGenerator &random_;
      int              grid_columns_ = 0;
      int              grid_rows_    = 0;
  };
} // namespace wheel_of_fear
