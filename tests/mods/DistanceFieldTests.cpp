#include "mods/world/DistanceField.hpp"
#include "mods/world/TileMapGenerator.hpp"
#include <doctest/doctest.h>

namespace wheel_of_fear {
  TEST_CASE("every room is reachable from the first room") {
    DistanceField   field;
    RandomGenerator random(42);
    TileMapModel    map;
    TileMapGenerator(map, random).generate(5, 4);

    REQUIRE(map.rooms().size() == 20);
    field.compute(map, map.randomFloorInRoom(random, map.rooms()[0]));

    for(const RoomProps &room : map.rooms()) {
      CHECK(
        field.distanceAt(map, map.randomFloorInRoom(random, room))
        != DistanceField::unreachable_distance
      );
    }
  }
} // namespace wheel_of_fear
