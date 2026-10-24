#include "support/HeadlessGameHarness.hpp"
#include <algorithm>
#include <cstddef>
#include <doctest/doctest.h>
#include <memory>
#include <unordered_map>

namespace wheel_of_fear {
  TEST_CASE("light holds a mob steadily instead of flickering its freeze") {
    int           most_releases     = 0;
    constexpr int frame_count       = 60 * 12;
    constexpr int aim_window_frames = 120;
    constexpr int allowed_releases  = (frame_count / aim_window_frames) + 1;

    const auto harness = std::make_unique<HeadlessGameHarness>("freeze");
    std::unordered_map<const MobEntity*, bool> was_frozen;
    std::unordered_map<const MobEntity*, int>  releases;

    for(int frame = 0; frame < frame_count; ++frame) {
      harness->advanceFrames(1);

      for(const auto &mob : harness->context().mobs.all()) {
        const bool is_frozen = mob->condition().isFrozen();

        if(was_frozen[mob.get()] && !is_frozen) {
          releases[mob.get()] += 1;
          most_releases = std::max(most_releases, releases[mob.get()]);
        }

        was_frozen[mob.get()] = is_frozen;
      }
    }

    CHECK(most_releases > 0);
    CHECK(most_releases <= allowed_releases);
  }
} // namespace wheel_of_fear
