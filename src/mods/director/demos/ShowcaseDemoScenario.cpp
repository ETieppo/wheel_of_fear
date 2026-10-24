#include "mods/director/demos/ShowcaseDemoScenario.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  void ShowcaseDemoScenario::run(
    GameContextDto                     &context,
    [[maybe_unused]] const DemoTickDto &tick
  ) {
    context.mobs.setGraceSeconds(1000);
    aimAtWorldOffset(context, Vec2Props::of(0, -100));
    if(step() != 0) return;

    setStep(1);
    context.mobs.clear();
    float offset = -66;

    for(const MobKindEnum kind : all_mob_kinds) {
      context.mobs.spawn(
        kind,
        context.world.map().nearestFloor(
          context.player.position().add(Vec2Props::of(offset, -52))
        )
      );

      offset += 22;
    }
  }
} // namespace wheel_of_fear
