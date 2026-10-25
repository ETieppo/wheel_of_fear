#include "mods/mobs/MobEntity.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include "support/HeadlessGameHarness.hpp"
#include <doctest/doctest.h>
#include <memory>
#include <optional>

namespace wheel_of_fear {
  namespace {
    std::optional<DoorwayProps> findOpenDoorwayFrom(
      const TileMapModel &map,
      std::size_t         lit_room
    ) {
      for(const DoorwayProps &doorway : map.doorways())
        if(
          doorway.is_open
          && doorway.open_amount
          >= 1
          && doorway.connects(lit_room)
        )
          return doorway;

      return std::nullopt;
    }

    Vec2Props pointPastDoorway(
      const TileMapModel &map,
      const DoorwayProps &doorway,
      std::size_t         lit_room,
      float               distance
    ) {
      const Vec2Props neighbor_center
        = map.rooms()[doorway.otherRoom(lit_room)].center();
      const Vec2Props inward
        = neighbor_center.sub(doorway.center()).normalized();

      return doorway.center().add(inward.scale(distance));
    }
  } // namespace

  TEST_CASE("powered rooms spill freezing light through open doors only") {
    const auto      harness = std::make_unique<HeadlessGameHarness>("play");
    GameContextDto &context = harness->context();
    WorldSystem    &world   = context.world;

    harness->advanceFrames(90);
    REQUIRE(context.clock.isRunning());

    context.player.flashlight().setEnabled(false);

    const std::size_t lit_room = world.startRoomIndex();

    world.powerRoom(lit_room, true);
    harness->advanceFrames(300);

    const std::optional<DoorwayProps> doorway
      = findOpenDoorwayFrom(world.map(), lit_room);
    REQUIRE(doorway.has_value());

    const Vec2Props mob_position
      = pointPastDoorway(world.map(), *doorway, lit_room, 36);
    const Vec2Props beyond_spill
      = pointPastDoorway(world.map(), *doorway, lit_room, 140);

    REQUIRE(
      world.map().roomIndexAt(mob_position) == doorway->otherRoom(lit_room)
    );
    CHECK(
      context.lighting
        .freezingLightAt(world.map(), mob_position, LightFilterEnum::any_light)
      > 0
    );

    if(world.map().roomIndexAt(beyond_spill) == doorway->otherRoom(lit_room))
      CHECK(
        context.lighting.freezingLightAt(
          world.map(),
          beyond_spill,
          LightFilterEnum::any_light
        )
        == 0
      );

    context.mobs.clear();
    context.mobs.spawn(MobKindEnum::crawler, mob_position);
    harness->advanceFrames(2);

    REQUIRE(context.mobs.all().size() == 1);

    const MobEntity &mob        = *context.mobs.all().front();
    const Vec2Props  held_place = mob.position();

    harness->advanceFrames(120);

    CHECK(mob.condition().isFrozen());
    CHECK(mob.position().distance(held_place) < 0.5F);
  }
} // namespace wheel_of_fear
