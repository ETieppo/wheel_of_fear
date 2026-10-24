#include "mods/director/demos/LockerDemoScenario.hpp"

#include <format>

#include "core/input/InputController.hpp"
#include "mods/camera/CameraSystem.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  void LockerDemoScenario::run(
    GameContextDto    &context,
    const DemoTickDto &tick
  ) {
    WorldSystem  &world  = context.world;
    PlayerEntity &player = context.player;

    context.mobs.setGraceSeconds(has_witness_ ? 0.0F : 1000.0F);
    if(has_witness_) aimAtWorldOffset(context, Vec2Props::of(0, -200));

    if(step() == 0 && !world.lockers().empty()) {
      setStep(1);

      std::size_t nearest = 0;
      const auto &lockers = world.lockers();

      for(std::size_t index = 0; index < lockers.size(); ++index)
        if(
          lockers[index].position().distance(player.position()) <
          lockers[nearest].position().distance(player.position())
        )
          nearest = index;

      player.setPosition(
        world.map().nearestFloor(lockers[nearest].entryPoint())
      );

      context.camera.snapToPlayer(world, player);
      context.mobs.clear();
      if(has_witness_) spawnInSight(context, MobKindEnum::crawler, 130);
    }

    if(step() == 1 && tick.seconds > 1.5F) {
      setStep(2);
      context.input.simulatePress(InputActionEnum::interact);
    }

    if(step() == 2 && tick.seconds > 4.5F && !has_witness_) {
      setStep(3);
      context.input.simulatePress(InputActionEnum::interact);
    }
    if(!tick.should_log) return;

    logLine(
      std::format(
        "locker: step={} hidden={} locker={}",
        step(),
        player.isHidden(),
        optionalIndex(player.hiding().hiddenLockerIndex())
      )
    );

    const auto &mobs = context.mobs.all();

    for(std::size_t index = 0; index < mobs.size(); ++index) {
      const MobEntity &mob = *mobs[index];

      logLine(
        std::format(
          "locker: mob {} intent={} known_locker={} distance={:.0f}",
          index,
          nameOf(mob.brain().intent()),
          optionalIndex(mob.perception().memory().known_locker_index),
          mob.distanceTo(player.position())
        )
      );
    }
  }
} // namespace wheel_of_fear
