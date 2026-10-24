#include <vector>

#include <doctest/doctest.h>

#include "mods/world/MazeShifter.hpp"

namespace wheel_of_fear {
  TEST_CASE(
    "shifting doorways never disconnects the rooms"
  ) {
    for(std::uint64_t seed = 0; seed < 6; ++seed) {
      RandomGenerator random(seed + 100);
      TileMap         map;
      map.generate(random, 5, 6);
      MazeShifter shifter;
      const Vec2  far_away = Vec2::of(-10000, -10000);
      for(int step = 0; step < 200; ++step) {
        const std::size_t focus = random.index(map.rooms().size());
        shifter.advance(map, random, 3, 30, far_away, focus, {});
        shifter.animateDoorways(map, 2, {});
        for(DoorwayProps &doorway : map.doorways())
          doorway.seconds_since_change += 20;
        REQUIRE(map.isRoomGraphConnected());
        if(step % 10 == 0) REQUIRE(map.isEveryFloorConnected());
      }
    }
  }

  TEST_CASE(
    "doorways inside the view are never moved"
  ) {
    RandomGenerator random(77);
    TileMap         map;
    map.generate(random, 4, 4);
    MazeShifter           shifter;
    constexpr std::size_t focus       = 5;
    const Vec2            view_center = map.rooms()[focus].center();
    std::vector<bool>     before;
    for(const DoorwayProps &doorway : map.doorways())
      before.push_back(doorway.is_open);
    for(int step = 0; step < 50; ++step)
      shifter.advance(map, random, 1, 30, view_center, focus, {});
    for(std::size_t index = 0; index < map.doorways().size(); ++index) {
      const DoorwayProps &doorway = map.doorways()[index];
      if(MazeShifter::isInsideView(doorway.center(), view_center))
        CHECK(before[index] == doorway.is_open);
    }
  }
} // namespace wheel_of_fear
