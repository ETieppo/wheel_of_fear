#include "mods/world/MazeShifter.hpp"
#include "mods/world/TileMapConnectivityAnalyzer.hpp"
#include "mods/world/TileMapGenerator.hpp"
#include <doctest/doctest.h>
#include <vector>

namespace wheel_of_fear {
  TEST_CASE("shifting doorways never disconnects the rooms") {
    for(std::uint64_t seed = 0; seed < 6; ++seed) {
      RandomGenerator random(seed + 100);
      TileMapModel    map;
      TileMapGenerator(map, random).generate(5, 6);
      MazeShifter     shifter;
      const Vec2Props far_away = Vec2Props::of(-10000, -10000);

      for(int step = 0; step < 200; ++step) {
        const std::size_t focus = random.index(map.rooms().size());

        shifter.advance(map, random, 3, 30, far_away, focus, {});
        MazeShifter::animateDoorways(map, 2, {});

        for(DoorwayProps &doorway : map.doorways())
          doorway.seconds_since_change += 20;

        REQUIRE(TileMapConnectivityAnalyzer::isRoomGraphConnected(map));
        if(step % 10 == 0)
          REQUIRE(TileMapConnectivityAnalyzer::isEveryFloorConnected(map));
      }
    }
  }

  TEST_CASE("doorways inside the view are never moved") {
    RandomGenerator random(77);
    TileMapModel    map;
    TileMapGenerator(map, random).generate(4, 4);
    MazeShifter shifter;

    constexpr std::size_t focus       = 5;
    const Vec2Props       view_center = map.rooms()[focus].center();
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

  TEST_CASE("a closed doorway next to the position opens and becomes passable") {
    RandomGenerator random(42);
    TileMapModel    map;
    TileMapGenerator(map, random).generate(4, 4);

    DoorwayProps *closed_doorway = nullptr;
    for(DoorwayProps &doorway : map.doorways())
      if(!doorway.is_open) closed_doorway = &doorway;

    REQUIRE(closed_doorway != nullptr);
    REQUIRE(map.isWall(closed_doorway->first_x, closed_doorway->first_y));

    MazeShifter::openDoorwaysNear(map, closed_doorway->center());
    MazeShifter::animateDoorways(map, 2, {});

    CHECK(closed_doorway->is_open);
    CHECK_FALSE(map.isWall(closed_doorway->first_x, closed_doorway->first_y));
  }

  TEST_CASE("doorways far from the position stay closed") {
    RandomGenerator random(42);
    TileMapModel    map;
    TileMapGenerator(map, random).generate(4, 4);
    std::vector<bool> before;

    for(const DoorwayProps &doorway : map.doorways())
      before.push_back(doorway.is_open);

    MazeShifter::openDoorwaysNear(map, Vec2Props::of(-10000, -10000));

    for(std::size_t index = 0; index < map.doorways().size(); ++index)
      CHECK(before[index] == map.doorways()[index].is_open);
  }
} // namespace wheel_of_fear
