#include <array>

#include <doctest/doctest.h>

#include "mods/camera/RoomFraming.hpp"

namespace wheel_of_fear {
  TEST_CASE(
    "the camera never shows past the walls of the current room"
  ) {
    const RoomProps room {31, 21, 29, 19, 1, 1};
    const Vec2      minimum = room.worldMinimum().sub(
      Vec2::of(RoomFraming::wall_margin, RoomFraming::wall_margin)
    );
    const Vec2 maximum = room.worldMaximum().add(
      Vec2::of(RoomFraming::wall_margin, RoomFraming::wall_margin)
    );
    const Vec2 half_view = RoomFraming::half_view;
    for(
      const Vec2 desired : std::array<Vec2, 4> {
        Vec2::of(-500, -500),
        Vec2::of(5000, 5000),
        room.center(),
        Vec2::of(room.center().x, -40)
      }
    ) {
      const Vec2 center = RoomFraming::clampToRoom(room, desired);
      CHECK(center.x - half_view.x >= minimum.x - 0.01F);
      CHECK(center.x + half_view.x <= maximum.x + 0.01F);
      CHECK(center.y - half_view.y >= minimum.y - 0.01F);
      CHECK(center.y + half_view.y <= maximum.y + 0.01F);
    }
  }
} // namespace wheel_of_fear
