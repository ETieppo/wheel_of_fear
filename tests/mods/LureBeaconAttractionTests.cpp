#include "mods/mobs/MobSystem.hpp"
#include "mods/throwables/ThrowableSystem.hpp"
#include "support/HeadlessGameHarness.hpp"
#include <algorithm>
#include <doctest/doctest.h>
#include <memory>

namespace wheel_of_fear {
  namespace {
    constexpr int frames_per_second = 60;

    const MobEntity &onlyMob(HeadlessGameHarness &harness) {
      return *harness.context().mobs.all().front();
    }
  } // namespace

  TEST_CASE("mobs stay on an active lure beacon and return after it expires") {
    const auto harness = std::make_unique<HeadlessGameHarness>("lure");
    for(
      int frame = 0; frame
      < frames_per_second
      * 15
      && harness->context().throwables.beacons().empty();
      ++frame
    )
      harness->advanceFrames(1);

    REQUIRE_FALSE(harness->context().throwables.beacons().empty());

    float closest_to_beacon = 1000;
    bool  stayed_lured      = true;

    while(!harness->context().throwables.beacons().empty()) {
      const Vec2Props beacon
        = harness->context().throwables.beacons().front().position();
      const MobEntity &mob = onlyMob(*harness);

      stayed_lured
        = stayed_lured && mob.brain().intent() == MobIntentEnum::lured;
      closest_to_beacon = std::min(closest_to_beacon, mob.distanceTo(beacon));
      harness->advanceFrames(1);
    }

    CHECK(stayed_lured);
    CHECK(closest_to_beacon < 25);

    harness->advanceFrames(frames_per_second / 2);
    CHECK(onlyMob(*harness).brain().intent() != MobIntentEnum::lured);
  }
} // namespace wheel_of_fear
