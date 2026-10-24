#include "mods/director/demos/LureDemoScenario.hpp"

#include <format>

#include "core/input/InputController.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/throwables/ThrowableSystem.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  void LureDemoScenario::run(
    GameContextDto    &context,
    const DemoTickDto &tick
  ) {
    PlayerEntity &player = context.player;

    context.mobs.setGraceSeconds(0);
    player.flashlight().setEnabled(false);
    aimAtWorldOffset(context, Vec2Props::of(120, 0));

    if(step() == 0) {
      setStep(1);
      context.mobs.clear();

      context.mobs.spawn(
        MobKindEnum::crawler,
        context.world.map().nearestFloor(
          player.position().add(Vec2Props::of(-330, 0))
        )
      );
    }

    if(step() == 1 && tick.seconds > 1) {
      setStep(2);
      context.input.simulatePress(InputActionEnum::throw_lure);
    }
    if(!tick.should_log || context.throwables.beacons().empty()) return;

    const Vec2Props beacon = context.throwables.beacons().front().position();
    const auto     &mobs   = context.mobs.all();

    for(std::size_t index = 0; index < mobs.size(); ++index) {
      const MobEntity &mob = *mobs[index];

      logLine(
        std::format(
          "lure: t={:.1f} mob {} intent={} "
          "distance_to_beacon={:.0f} distance_to_player={:.0f}",
          tick.seconds,
          index,
          nameOf(mob.brain().intent()),
          mob.distanceTo(beacon),
          mob.distanceTo(player.position())
        )
      );
    }
  }
} // namespace wheel_of_fear
