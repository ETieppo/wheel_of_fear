#include "mods/mobs/MobSystem.hpp"
#include <doctest/doctest.h>

namespace wheel_of_fear {
  namespace {
    MobEntity &spawnCrawler(
      MobSystem &mobs,
      Vec2Props  position
    ) {
      mobs.spawn(MobKindEnum::crawler, position);
      return *mobs.all().back();
    }
  } // namespace

  TEST_CASE("a mob hidden in the dark cannot be inspected") {
    MobSystem       mobs;
    const Vec2Props position = Vec2Props::of(100, 100);
    spawnCrawler(mobs, position).appearance().setVisibility(0);

    CHECK(mobs.visibleMobAt(position) == nullptr);
  }

  TEST_CASE("a visible mob under the cursor is inspected by name") {
    MobSystem       mobs;
    const Vec2Props position = Vec2Props::of(100, 100);
    spawnCrawler(mobs, position).appearance().setVisibility(1);

    const MobEntity* inspected
      = mobs.visibleMobAt(position.add(Vec2Props::of(3, 0)));

    REQUIRE(inspected != nullptr);
    CHECK(inspected->traits().display_name.english == "Crawler");
    CHECK(mobs.visibleMobAt(Vec2Props::of(300, 300)) == nullptr);
  }
} // namespace wheel_of_fear
