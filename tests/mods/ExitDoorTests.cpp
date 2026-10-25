#include "mods/generators/GeneratorSystem.hpp"
#include "mods/world/WorldSystem.hpp"
#include "support/HeadlessGameHarness.hpp"
#include <doctest/doctest.h>
#include <memory>

namespace wheel_of_fear {
  namespace {
    bool isExitBlocked(const GameContextDto &context) {
      const Vec2Props           exit_position = context.world.exitPosition();
      const TileCoordinateProps tile = TileMapModel::tileAt(exit_position);

      return context.world.map().isBlocked(tile.x - 1, tile.y)
        && context.world.map().isBlocked(tile.x, tile.y)
        && context.world.map().isBlocked(tile.x + 1, tile.y);
    }
  } // namespace

  TEST_CASE("the exit door blocks the way until it opens") {
    const auto      harness = std::make_unique<HeadlessGameHarness>("play");
    GameContextDto &context = harness->context();

    harness->advanceFrames(90);
    REQUIRE(context.clock.isRunning());
    CHECK(isExitBlocked(context));

    context.generators.exitDoor().unlock(context);
    harness->advanceFrames(150);

    REQUIRE(context.generators.exitDoor().openAmount() > 0.6F);
    CHECK_FALSE(isExitBlocked(context));
  }
} // namespace wheel_of_fear
