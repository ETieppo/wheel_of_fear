#include "support/HeadlessGameHarness.hpp"
#include <doctest/doctest.h>
#include <memory>

namespace wheel_of_fear {
  namespace {
    bool isOnDoorwayLine(
      const DoorwayProps       &doorway,
      const TileCoordinateProps tile
    ) {
      if(doorway.is_vertical)
        return tile.x
          == doorway.first_x
          && tile.y
          >= doorway.first_y
          - 1
          && tile.y
          <= doorway.first_y
          + DoorwayProps::length;

      return tile.y
        == doorway.first_y
        && tile.x
        >= doorway.first_x
        - 1
        && tile.x
        <= doorway.first_x
        + DoorwayProps::length;
    }
  } // namespace

  TEST_CASE("doorways never glow, so a shifted maze hides where doors were") {
    const auto harness = std::make_unique<HeadlessGameHarness>("doors");
    harness->advanceFrames(240);

    const TileMapModel &map = harness->context().world.map();
    REQUIRE_FALSE(map.doorways().empty());

    int visible_doorways = 0;
    for(const DoorwayProps &doorway : map.doorways()) {
      if(!harness->context().renderer.isVisible(doorway.center(), 40)) continue;

      visible_doorways += 1;

      for(const QuadProps &quad : harness->frame().glow.used()) {
        const TileCoordinateProps tile = TileMapModel::tileAt(
          Vec2Props::of(quad.position_size[0], quad.position_size[1])
        );

        CAPTURE(tile.x);
        CAPTURE(tile.y);
        CHECK_FALSE(isOnDoorwayLine(doorway, tile));
      }
    }

    CHECK(visible_doorways > 0);
  }
} // namespace wheel_of_fear
