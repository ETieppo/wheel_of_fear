#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include "support/HeadlessGameHarness.hpp"
#include <doctest/doctest.h>
#include <memory>

namespace wheel_of_fear {
  TEST_CASE("a powered room lights every floor tile, even behind inner walls") {
    const auto      harness = std::make_unique<HeadlessGameHarness>("play");
    GameContextDto &context = harness->context();
    WorldSystem    &world   = context.world;

    harness->advanceFrames(90);
    REQUIRE(context.clock.isRunning());

    context.player.flashlight().setEnabled(false);

    for(std::size_t room = 0; room < world.map().rooms().size(); ++room)
      world.powerRoom(room, true);

    harness->advanceFrames(300);

    const TileMapModel &map       = world.map();
    int                 dark_tile = 0;

    for(
      std::size_t room_index = 0; room_index < map.rooms().size(); ++room_index
    ) {
      const RoomProps &room = map.rooms()[room_index];

      for(int y = room.top; y < room.top + room.height; ++y)
        for(int x = room.left; x < room.left + room.width; ++x) {
          if(map.roomIndexAtTile(x, y) != room_index || map.isWall(x, y))
            continue;

          const float light = context.lighting.visibilityAt(
            map,
            TileMapModel::tileCenter(x, y),
            LightFilterEnum::any_light
          );

          if(light <= 0) dark_tile += 1;
        }
    }

    CHECK(dark_tile == 0);
  }
} // namespace wheel_of_fear
