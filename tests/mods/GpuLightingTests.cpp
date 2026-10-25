#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include "support/HeadlessGameHarness.hpp"
#include "support/OffscreenFrameCapture.hpp"
#include <doctest/doctest.h>
#include <memory>
#include <optional>

namespace wheel_of_fear {
  namespace {
    struct BrightnessAverageProps {
        float sum   = 0;
        int   count = 0;

        void add(float brightness) {
          sum += brightness;
          count += 1;
        }

        [[nodiscard]]
        float average() const {
          return count > 0 ? sum / static_cast<float>(count) : 0;
        }
    };

    std::optional<DoorwayProps> findOpenDoorway(
      const TileMapModel &map,
      std::size_t         room_index
    ) {
      for(const DoorwayProps &doorway : map.doorways())
        if(doorway.is_open && doorway.connects(room_index)) return doorway;

      return std::nullopt;
    }

    Vec2Props awayFromDoorway(
      const TileMapModel &map,
      const DoorwayProps &doorway,
      std::size_t         room_index,
      float               distance
    ) {
      const Vec2Props across
        = doorway.is_vertical ? Vec2Props::of(1, 0) : Vec2Props::of(0, 1);
      const float side
        = across.dot(map.rooms()[room_index].center().sub(doorway.center()))
          >= 0
        ? 1.0F
        : -1.0F;

      return doorway.center().add(across.scale(distance * side));
    }
  } // namespace

  TEST_CASE("a powered room shows the floor hidden from the player as lit") {
    const auto capture = OffscreenFrameCapture::tryCreate();

    if(!capture) {
      MESSAGE("no GPU adapter available, skipping the rendered check");
      return;
    }

    const auto      harness = std::make_unique<HeadlessGameHarness>("play");
    GameContextDto &context = harness->context();

    harness->advanceFrames(90);
    context.player.flashlight().setEnabled(false);

    const std::size_t lit_room = context.world.startRoomIndex();

    context.world.powerRoom(lit_room, true);
    harness->advanceFrames(300);
    capture->capture(harness->renderer());

    const TileMapModel    &map  = context.world.map();
    const RoomProps       &room = map.rooms()[lit_room];
    BrightnessAverageProps hidden;
    BrightnessAverageProps visible;

    for(int y = room.top; y < room.top + room.height; ++y)
      for(int x = room.left; x < room.left + room.width; ++x) {
        if(map.roomIndexAtTile(x, y) != lit_room || map.isBlocked(x, y))
          continue;

        const Vec2Props            center     = TileMapModel::tileCenter(x, y);
        const std::optional<float> brightness = capture->brightnessAt(center);
        if(!brightness) continue;

        if(map.hasLineOfSight(context.player.position(), center))
          visible.add(*brightness);
        else hidden.add(*brightness);
      }

    REQUIRE(visible.count > 0);
    REQUIRE(hidden.count > 0);
    CHECK(hidden.average() > visible.average() * 0.5F);
  }

  TEST_CASE("a powered room spills visible light through its open doors") {
    const auto capture = OffscreenFrameCapture::tryCreate();

    if(!capture) {
      MESSAGE("no GPU adapter available, skipping the rendered check");
      return;
    }

    const auto      harness = std::make_unique<HeadlessGameHarness>("play");
    GameContextDto &context = harness->context();
    WorldSystem    &world   = context.world;

    harness->advanceFrames(90);
    context.player.flashlight().setEnabled(false);

    const std::size_t                 lit_room = world.startRoomIndex();
    const std::optional<DoorwayProps> doorway
      = findOpenDoorway(world.map(), lit_room);
    REQUIRE(doorway.has_value());

    const std::size_t neighbor_room = doorway->otherRoom(lit_room);
    const Vec2Props   past_door     = awayFromDoorway(
      world.map(),
      *doorway,
      neighbor_room,
      RenderConfig::tile_size * 2.5F
    );

    context.player.setPosition(world.map().nearestFloor(
      awayFromDoorway(world.map(), *doorway, lit_room, 40)
    ));
    harness->advanceFrames(30);
    capture->capture(harness->renderer());

    const std::optional<float> dark = capture->brightnessAt(past_door);

    world.powerRoom(lit_room, true);
    harness->advanceFrames(300);
    capture->capture(harness->renderer());

    const std::optional<float> spilled = capture->brightnessAt(past_door);

    REQUIRE(world.map().roomIndexAt(past_door) == neighbor_room);
    REQUIRE(dark.has_value());
    REQUIRE(spilled.has_value());
    CHECK(*spilled > 0.25F);
    CHECK(*spilled - *dark > 0.2F);
  }
} // namespace wheel_of_fear
