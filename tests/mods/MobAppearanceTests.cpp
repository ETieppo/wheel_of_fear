#include <algorithm>
#include <cstddef>
#include <memory>

#include <doctest/doctest.h>

#include "support/HeadlessGameHarness.hpp"

namespace wheel_of_fear {
  namespace {
    MobEntity* findFrozenMob(
      HeadlessGameHarness &harness
    ) {
      for(int frame = 0; frame < 900; ++frame) {
        harness.advanceFrames(1);

        for(const auto &mob : harness.context().mobs.all())
          if(
            mob->condition().isFrozen() &&
            !mob->condition().isBeingHugged() &&
            mob->appearance().presence() > 0.5F
          )
            return mob.get();
      }

      return nullptr;
    }

    bool drawsBodyExactlyAtPosition(
      HeadlessGameHarness &harness,
      const MobEntity     &mob
    ) {
      const FrameProps &frame        = harness.frame();
      const std::size_t before_count = frame.world.count();

      mob.draw(harness.context());

      const auto quads = frame.world.used().subspan(before_count);

      return std::ranges::any_of(quads, [&mob](const QuadProps &quad) {
        return quad.position_size[0] ==
               mob.position().x &&
               quad.position_size[1] == mob.position().y;
      });
    }
  } // namespace

  TEST_CASE(
    "a mob held still by light is drawn perfectly still"
  ) {
    const auto harness = std::make_unique<HeadlessGameHarness>("freeze");
    MobEntity* mob     = findFrozenMob(*harness);

    REQUIRE(mob != nullptr);

    for(int sample = 0; sample < 5; ++sample) {
      REQUIRE(mob->condition().isFrozen());
      CHECK(drawsBodyExactlyAtPosition(*harness, *mob));
      harness->advanceFrames(1);
    }
  }
} // namespace wheel_of_fear
