#include "core/render/RenderConfig.hpp"
#include "mods/world/TileMapConnectivityAnalyzer.hpp"
#include "mods/world/TileMapGenerator.hpp"
#include "mods/world/TileMapModel.hpp"
#include <doctest/doctest.h>

namespace wheel_of_fear {
  TEST_CASE("sliding never ends inside a wall or prop") {
    RandomGenerator random(7);
    TileMapModel    map;
    TileMapGenerator(map, random).generate(4, 3);

    map.reserveProp(
      map.rooms()[0].centerTileX(),
      map.rooms()[0].centerTileY(),
      2,
      2
    );

    Vec2Props position = map.randomFloorInRoom(random, map.rooms()[0]);

    for(int step = 0; step < 3000; ++step) {
      const Vec2Props direction = random.direction();

      position = map.slide(position, direction.scale(random.range(0, 12)), 4);
      REQUIRE_FALSE(map.isSolidAt(position));
    }
  }

  TEST_CASE("generated maps keep every floor tile and every room reachable") {
    for(std::uint64_t seed = 0; seed < 8; ++seed) {
      RandomGenerator random(seed + 31);
      TileMapModel    map;
      TileMapGenerator(map, random).generate(5, 7);

      CHECK(map.width() <= static_cast<int>(RenderConfig::max_map_size));
      CHECK(map.height() <= static_cast<int>(RenderConfig::max_map_size));
      CHECK(TileMapConnectivityAnalyzer::isRoomGraphConnected(map));
      CHECK(TileMapConnectivityAnalyzer::isEveryFloorConnected(map));
    }
  }

  TEST_CASE("room boundary walls cannot be dissolved") {
    RandomGenerator random(5);
    TileMapModel    map;
    TileMapGenerator(map, random).generate(3, 3);

    const RoomProps room = map.rooms()[0];
    CHECK_FALSE(map.dissolveWall(room.left + room.width, room.top + 2));
    CHECK_FALSE(map.dissolveWall(room.left + 2, room.top + room.height));
  }
} // namespace wheel_of_fear
