#include <doctest/doctest.h>

#include "mods/world/DistanceField.hpp"

namespace wheel_of_fear {
  TEST_CASE(
    "every room is reachable from the first room"
  ) {
    RandomGenerator random(42);
    TileMap         map;
    map.generate(random, 5, 4);
    REQUIRE(map.rooms().size() == 20);
    DistanceField field;
    field.compute(map, map.randomFloorInRoom(random, map.rooms()[0]));
    for(const RoomProps &room : map.rooms()) {
      CHECK(
        field.distanceAt(map, map.randomFloorInRoom(random, room)) !=
        DistanceField::unreachable_distance
      );
    }
  }
} // namespace wheel_of_fear
