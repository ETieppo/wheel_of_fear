#include "core/input/InputController.hpp"
#include "core/time/GameClock.hpp"
#include "mods/generators/GeneratorSystem.hpp"
#include "mods/letters/LetterCatalog.hpp"
#include "mods/letters/LetterSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/TileMapConnectivityAnalyzer.hpp"
#include "mods/world/WorldSystem.hpp"
#include "support/HeadlessGameHarness.hpp"
#include <cstdint>
#include <doctest/doctest.h>
#include <memory>

namespace wheel_of_fear {
  namespace {
    constexpr int frames_per_second = 60;

    void advanceUntilPlaying(HeadlessGameHarness &harness) {
      for(
        int frame = 0;
        frame < frames_per_second * 10 && !harness.context().clock.isRunning();
        ++frame
      )
        harness.advanceFrames(1);
    }
  } // namespace

  TEST_CASE(
    "every sector hides its letter on reachable floor away from generators"
  ) {
    for(std::uint32_t phase = 1; phase <= 12; ++phase) {
      for(std::uint64_t seed = 0; seed < 4; ++seed) {
        const auto world = std::make_unique<WorldSystem>();
        world->generatePhase(phase, 0x1E77 + (seed * 7919));
        GeneratorSystem generators;
        generators.generatePhase(*world);
        LetterSystem letters;
        letters.generatePhase(*world, generators, seed);

        CAPTURE(phase);
        CAPTURE(seed);
        REQUIRE(letters.letter().has_value());

        const TileMapModel       &map  = world->map();
        const Vec2Props           spot = letters.letter()->position();
        const TileCoordinateProps tile = TileMapModel::tileAt(spot);
        const auto                room = map.roomIndexAt(spot);

        CHECK_FALSE(map.isWall(tile.x, tile.y));
        CHECK_FALSE(map.isBlocked(tile.x, tile.y));
        CHECK(TileMapConnectivityAnalyzer::isEveryFloorConnected(map));
        REQUIRE(room.has_value());
        CHECK(*room != world->startRoomIndex());
        CHECK(*room != world->exitRoomIndex());

        for(const auto &generator : generators.all())
          CHECK(map.roomIndexAt(generator.position()) != room);
      }
    }
  }

  TEST_CASE("the letters of every sector exist in both languages") {
    CHECK(LetterCatalog::load(LanguageEnum::english).size() == 12);
    CHECK(LetterCatalog::load(LanguageEnum::brazilian_portuguese).size() == 12);
  }

  TEST_CASE("the exit stays sealed until the key from the letter is found") {
    const auto harness = std::make_unique<HeadlessGameHarness>("play");
    advanceUntilPlaying(*harness);
    GameContextDto &context = harness->context();

    for(GeneratorEntity &generator : context.generators.all())
      generator.forceOnline(context);
    harness->advanceFrames(10);
    CHECK_FALSE(context.generators.exitDoor().isUnlocked());

    context.letters.grantKey();
    harness->advanceFrames(10);
    CHECK(context.generators.exitDoor().isUnlocked());
  }

  TEST_CASE(
    "reading a letter pauses the game and hands over the key and a gift"
  ) {
    const auto harness = std::make_unique<HeadlessGameHarness>("play");
    advanceUntilPlaying(*harness);
    GameContextDto &context = harness->context();
    REQUIRE(context.letters.letter().has_value());

    const int grenades_before
      = context.player.inventory().count(ItemEnum::neon_grenade);
    context.player.setPosition(context.letters.letter()->position());
    harness->advanceFrames(2);
    harness->advanceFramePressing(InputActionEnum::interact);
    harness->advanceFrames(3);
    CHECK_FALSE(context.clock.isRunning());
    CHECK_FALSE(context.letters.hasKey());

    harness->advanceFrames(frames_per_second);
    harness->advanceFramePressing(InputActionEnum::confirm);
    harness->advanceFrames(2);

    CHECK(context.clock.isRunning());
    CHECK(context.letters.hasKey());
    CHECK_FALSE(context.letters.letter().has_value());
    CHECK(
      context.player.inventory().count(ItemEnum::neon_grenade)
      == grenades_before
      + 1
    );
  }
} // namespace wheel_of_fear
