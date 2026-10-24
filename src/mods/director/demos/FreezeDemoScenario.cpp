#include "mods/director/demos/FreezeDemoScenario.hpp"
#include "core/math/ScalarMath.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include <format>

namespace wheel_of_fear {
  void FreezeDemoScenario::run(
    GameContextDto    &context,
    const DemoTickDto &tick
  ) {
    const PlayerEntity &player = context.player;
    context.mobs.setGraceSeconds(0);

    if(step() == 0) {
      setStep(1);
      context.mobs.clear();

      context.mobs.spawn(
        MobKindEnum::crawler,
        context.world.map().nearestFloor(
          player.position().add(Vec2Props::of(70, 0))
        )
      );

      context.mobs.spawn(
        MobKindEnum::crawler,
        context.world.map().nearestFloor(
          player.position().add(Vec2Props::of(-70, 0))
        )
      );
    }

    aimAtWorldOffset(
      context,
      Vec2Props::of(
        ScalarMath::floorModulo(tick.seconds, 4) < 2 ? 160.0F : -160.0F,
        0
      )
    );

    if(!tick.should_log) return;
    const auto &mobs = context.mobs.all();

    for(std::size_t index = 0; index < mobs.size(); ++index) {
      const MobEntity &mob = *mobs[index];

      logLine(
        std::format(
          "freeze: mob {} side={} frozen={} intent={} distance={:.0f}",
          index,
          mob.position().x > player.position().x ? "right" : "left",
          mob.condition().isFrozen(),
          nameOf(mob.brain().intent()),
          mob.distanceTo(player.position())
        )
      );
    }
  }
} // namespace wheel_of_fear
