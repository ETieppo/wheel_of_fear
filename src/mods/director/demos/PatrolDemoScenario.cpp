#include "mods/director/demos/PatrolDemoScenario.hpp"

#include <format>

#include "mods/mobs/MobSystem.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  void PatrolDemoScenario::run(
    GameContextDto    &context,
    const DemoTickDto &tick
  ) {
    if(
      !tick.should_log ||
      tick.seconds <=
      static_cast<float>(step()) *
      log_step_seconds
    )
      return;

    setStep(step() + 1);
    logPatrols(context, tick.seconds);
  }

  void PatrolDemoScenario::logPatrols(
    GameContextDto &context,
    float           seconds
  ) {
    const auto &mobs = context.mobs.all();

    for(std::size_t index = 0; index < mobs.size(); ++index) {
      const MobEntity &mob = *mobs[index];

      logLine(
        std::format(
          "patrol t={:.0f} sector={} mob {} {} room={} intent={}",
          seconds,
          context.world.phase(),
          index,
          nameOf(mob.kind()),
          optionalIndex(context.world.map().roomIndexAt(mob.position())),
          nameOf(mob.brain().intent())
        )
      );
    }
  }
} // namespace wheel_of_fear
