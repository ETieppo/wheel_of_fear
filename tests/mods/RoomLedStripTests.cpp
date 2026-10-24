#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>

#include <doctest/doctest.h>

#include "mods/world/RoomLedStrip.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  namespace {
    std::optional<DoorwayProps> doorwayAt(
      const TileMapModel       &map,
      const TileCoordinateProps tile
    ) {
      for(const DoorwayProps &doorway : map.doorways())
        for(int step = 0; step < DoorwayProps::length; ++step)
          if(doorway.tileAt(step) == tile) return doorway;

      return std::nullopt;
    }

    TileCoordinateProps tileBeyond(
      const RoomLedPlacementProps &led
    ) {
      return TileMapModel::tileAt(
        led.position.add(led.outward.scale(RoomLedStrip::inset + 2))
      );
    }

    void checkStripsHugOutline(
      std::uint32_t phase,
      std::uint64_t seed
    ) {
      const auto world = std::make_unique<WorldSystem>();

      world->generatePhase(phase, seed);

      const TileMapModel &map = world->map();

      for(
        std::size_t room_index = 0; room_index < map.rooms().size();
        ++room_index
      ) {
        CAPTURE(phase);
        CAPTURE(seed);
        CAPTURE(room_index);

        const RoomLedStrip strip = RoomLedStrip::trace(map, room_index);

        REQUIRE(strip.length() > 0);

        for(const RoomLedPlacementProps &led : strip.placements(map)) {
          const TileCoordinateProps inside = TileMapModel::tileAt(led.position);
          const TileCoordinateProps beyond = tileBeyond(led);

          CHECK(map.roomIndexAtTile(inside.x, inside.y) == room_index);
          CHECK(map.isStructural(beyond.x, beyond.y));
        }
      }
    }
  } // namespace

  TEST_CASE(
    "the LED strip hugs the real outline of every room"
  ) {
    for(std::uint32_t phase = 1; phase <= 6; ++phase)
      for(std::uint64_t seed = 0; seed < 4; ++seed)
        checkStripsHugOutline(phase, 0x5151 + (seed * 104729));
  }

  TEST_CASE(
    "the LED strip leaves a gap only in front of open doorways"
  ) {
    const auto world = std::make_unique<WorldSystem>();

    world->generatePhase(4, 0x5151);

    const TileMapModel &map                = world->map();
    int                 leds_facing_open   = 0;
    int                 leds_facing_closed = 0;

    for(
      std::size_t room_index = 0; room_index < map.rooms().size(); ++room_index
    ) {
      const RoomLedStrip strip = RoomLedStrip::trace(map, room_index);

      for(const RoomLedPlacementProps &led : strip.placements(map)) {
        const auto doorway = doorwayAt(map, tileBeyond(led));

        if(!doorway) {
          CHECK_FALSE(led.faces_open_passage);
          continue;
        }

        CHECK(led.faces_open_passage == doorway->is_open);

        if(doorway->is_open) leds_facing_open += 1;
        else leds_facing_closed += 1;
      }
    }

    CHECK(leds_facing_open > 0);
    CHECK(leds_facing_closed > 0);
  }
} // namespace wheel_of_fear
