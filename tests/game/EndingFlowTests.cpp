#include "mods/world/WorldSystem.hpp"
#include "support/HeadlessGameHarness.hpp"
#include "support/OffscreenFrameCapture.hpp"
#include <doctest/doctest.h>
#include <memory>

namespace wheel_of_fear {
  namespace {
    constexpr int frames_per_second   = 60;
    constexpr int maximum_roll_frames = frames_per_second * 120;
    constexpr int minimum_roll_frames = frames_per_second * 20;
  } // namespace

  TEST_CASE("the ending walks outdoors in daylight and returns to the title") {
    const auto harness = std::make_unique<HeadlessGameHarness>("ending");
    harness->advanceFrames(2);
    REQUIRE(harness->context().world.isOutdoors());

    const Vec2Props start = harness->context().player.position();
    harness->advanceFrames(frames_per_second * 2);
    CHECK(harness->context().player.position().y < start.y);

    int roll_frames = 0;

    while(
      roll_frames < maximum_roll_frames && harness->context().world.isOutdoors()
    ) {
      harness->advanceFrames(1);
      roll_frames += 1;
    }

    CHECK_FALSE(harness->context().world.isOutdoors());
    CHECK(roll_frames > minimum_roll_frames);
  }

  TEST_CASE("the ending clearing is rendered in daylight") {
    const auto capture = OffscreenFrameCapture::tryCreate();

    if(!capture) {
      MESSAGE("no GPU adapter available, skipping the rendered check");
      return;
    }

    const auto harness = std::make_unique<HeadlessGameHarness>("ending");
    harness->advanceFrames(frames_per_second);
    capture->capture(harness->renderer());

    const Vec2Props beside_player
      = harness->context().player.position().add(Vec2Props::of(60, 0));
    const auto brightness = capture->brightnessAt(beside_player);

    REQUIRE(brightness.has_value());
    CHECK(*brightness > 0.2F);
  }
} // namespace wheel_of_fear
