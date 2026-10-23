#pragma once

#include <cstddef>

#include "core/math/Color.hpp"
#include "mods/world/RoomProps.hpp"
#include "mods/world/enums/SceneryKindEnum.hpp"

namespace wheel_of_fear {
  class World;

  class WorldFurnisher {
    public:
      explicit WorldFurnisher(World &world)
          : world_(world) {
      }

      void furnishRoom(
        const RoomProps &room,
        std::size_t      room_index,
        Color            color
      );
      void placeLockers(
        const RoomProps &room,
        std::size_t      room_index
      );
      void placeNeonSigns(
        const RoomProps &room,
        std::size_t      room_index
      );

    private:
      bool tryReserveScenery(
        const RoomProps &room,
        std::size_t      room_index,
        int              x,
        int              y,
        SceneryKindEnum  kind,
        Color            color
      );
      int interiorX(const RoomProps &room);
      int interiorY(const RoomProps &room);
      static
      bool isInClearing(
        const RoomProps &room,
        int              x,
        int              y
      );

      World &world_;
  };
} // namespace wheel_of_fear
