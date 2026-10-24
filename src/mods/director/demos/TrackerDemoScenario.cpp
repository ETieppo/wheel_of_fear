#include "mods/director/demos/TrackerDemoScenario.hpp"

#include "mods/tracker/TrackerSystem.hpp"

namespace wheel_of_fear {
  void TrackerDemoScenario::run(
    GameContextDto    &context,
    const DemoTickDto &tick
  ) {
    context.tracker.forceOpen(tick.seconds > open_after_seconds);
  }
} // namespace wheel_of_fear
