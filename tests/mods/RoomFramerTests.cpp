#include <array>

#include <doctest/doctest.h>

#include "mods/camera/RoomFramer.hpp"

namespace wheel_of_fear {
  TEST_CASE(
    "the camera never shows past the walls of the current room"
  ) {
    const RoomProps room {
      .left   = 31,
      .top    = 21,
      .width  = 29,
      .height = 19,
      .column = 1,
      .row    = 1,
    };

    const Vec2Props minimum = room.worldMinimum().sub(
      Vec2Props::of(RoomFramer::wall_margin, RoomFramer::wall_margin)
    );

    const Vec2Props maximum = room.worldMaximum().add(
      Vec2Props::of(RoomFramer::wall_margin, RoomFramer::wall_margin)
    );

    const Vec2Props half_view = RoomFramer::half_view;

    for(
      const Vec2Props desired : std::array<Vec2Props, 4> {
        Vec2Props::of(-500, -500),
        Vec2Props::of(5000, 5000),
        room.center(),
        Vec2Props::of(room.center().x, -40),
      }
    ) {
      const Vec2Props center = RoomFramer::clampToRoom(room, desired);

      CHECK(center.x - half_view.x >= minimum.x - 0.01F);
      CHECK(center.x + half_view.x <= maximum.x + 0.01F);
      CHECK(center.y - half_view.y >= minimum.y - 0.01F);
      CHECK(center.y + half_view.y <= maximum.y + 0.01F);
    }
  }
} // namespace wheel_of_fear
