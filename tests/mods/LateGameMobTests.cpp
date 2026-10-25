#include "mods/mobs/MobSystem.hpp"
#include "mods/throwables/ThrowableSystem.hpp"
#include "mods/world/WorldSystem.hpp"
#include "support/HeadlessGameHarness.hpp"
#include <algorithm>
#include <cstdint>
#include <doctest/doctest.h>
#include <memory>

namespace wheel_of_fear {
  namespace {
    bool populatesKind(
      std::uint32_t phase,
      MobKindEnum   kind
    ) {
      const auto world = std::make_unique<WorldSystem>();
      world->generatePhase(phase, 0xD00D + phase);
      MobSystem mobs;
      mobs.populatePhase(*world);

      return std::ranges::any_of(mobs.all(), [kind](const auto &mob) {
        return mob->kind() == kind;
      });
    }
  } // namespace

  TEST_CASE("the watcher and the devourer always debut in their own sector") {
    CHECK(populatesKind(11, MobKindEnum::watcher));
    CHECK(populatesKind(12, MobKindEnum::devourer));
    CHECK_FALSE(populatesKind(10, MobKindEnum::watcher));
    CHECK_FALSE(populatesKind(11, MobKindEnum::devourer));
  }

  TEST_CASE("the devourer eats lure beacons and grenade puddles") {
    const auto world = std::make_unique<WorldSystem>();
    world->generatePhase(12, 0xBEEF);
    ThrowableSystem throwables;

    const Vec2Props           spot = world->playerStart();
    const TileCoordinateProps tile = TileMapModel::tileAt(spot);
    throwables.deployBeacon(spot);
    throwables.spillPuddle(spot);
    world->map().fluids().spillNeon(tile.x, tile.y, 30);

    REQUIRE(throwables.nearestTool(spot.add(Vec2Props::of(40, 0)), 100));
    CHECK(throwables.devourAt(*world, spot, 16));
    CHECK(throwables.beacons().empty());
    CHECK(throwables.puddleCount() == 0);
    CHECK(world->map().fluids().neonAt(spot) == 0);
    CHECK_FALSE(throwables.nearestTool(spot, 100));
  }

  TEST_CASE("a watcher scream pulls other mobs but not other watchers") {
    MobSystem       mobs;
    const Vec2Props caller = Vec2Props::of(100, 100);
    mobs.spawn(MobKindEnum::crawler, Vec2Props::of(900, 900));
    mobs.spawn(MobKindEnum::watcher, Vec2Props::of(120, 100));

    mobs.answerCall(
      MobKindEnum::watcher,
      caller,
      Vec2Props::of(50, 50),
      100000,
      6
    );

    const MobEntity &crawler = *mobs.all()[0];
    const MobEntity &watcher = *mobs.all()[1];

    CHECK(crawler.perception().memory().noise_position.has_value());
    CHECK_FALSE(watcher.perception().memory().noise_position.has_value());
  }

  TEST_CASE("a devourer walks to a live lure beacon and eats it") {
    const auto harness = std::make_unique<HeadlessGameHarness>("lure");

    for(
      int frame = 0;
      frame < 60 * 15 && harness->context().throwables.beacons().empty();
      ++frame
    )
      harness->advanceFrames(1);

    REQUIRE_FALSE(harness->context().throwables.beacons().empty());

    GameContextDto &context = harness->context();
    const Vec2Props beacon  = context.throwables.beacons().front().position();
    context.mobs.clear();
    context.mobs.spawn(
      MobKindEnum::devourer,
      context.world.map().nearestFloor(beacon.add(Vec2Props::of(80, 0)))
    );

    for(
      int frame = 0; frame < 60 * 6 && !context.throwables.beacons().empty();
      ++frame
    )
      harness->advanceFrames(1);

    CHECK(context.throwables.beacons().empty());
  }
} // namespace wheel_of_fear
