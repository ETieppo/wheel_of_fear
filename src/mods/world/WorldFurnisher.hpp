#pragma once

#include <cstddef>

#include "core/math/ColorProps.hpp"
#include "mods/world/RoomProps.hpp"
#include "mods/world/enums/SceneryKindEnum.hpp"

namespace wheel_of_fear {
  class WorldSystem;

  class WorldFurnisher {
    public:
      explicit WorldFurnisher(
        WorldSystem &world
      )
          : world_(world) {
      }

      void furnishRoom(
        const RoomProps &room,
        std::size_t      room_index,
        ColorProps       color
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
      void furnishServerFarm(
        const RoomProps &room,
        std::size_t      room_index,
        ColorProps       color
      );
      void furnishStorage(
        const RoomProps &room,
        std::size_t      room_index,
        ColorProps       color
      );
      void furnishOffices(
        const RoomProps &room,
        std::size_t      room_index,
        ColorProps       color
      );
      void furnishPumpStation(
        const RoomProps &room,
        std::size_t      room_index,
        ColorProps       color
      );
      void furnishHall(
        const RoomProps &room,
        std::size_t      room_index,
        ColorProps       color
      );
      bool tryReserveScenery(
        const RoomProps &room,
        std::size_t      room_index,
        int              x,
        int              y,
        SceneryKindEnum  kind,
        ColorProps       color
      );
      int interiorX(
        const RoomProps &room
      );
      int interiorY(
        const RoomProps &room
      );
      static bool isInClearing(
        const RoomProps &room,
        int              x,
        int              y
      );

      WorldSystem &world_;
  };
} // namespace wheel_of_fear
