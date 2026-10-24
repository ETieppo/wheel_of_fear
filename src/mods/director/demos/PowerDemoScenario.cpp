#include "mods/director/demos/PowerDemoScenario.hpp"

#include <format>

#include "core/input/InputController.hpp"
#include "mods/camera/CameraSystem.hpp"
#include "mods/generators/GeneratorSystem.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  void PowerDemoScenario::run(
    GameContextDto    &context,
    const DemoTickDto &tick
  ) {
    context.mobs.setGraceSeconds(1000);
    if(context.generators.all().empty()) return;

    GeneratorEntity &generator = context.generators.all().front();

    if(step() == 0) {
      setStep(1);

      context.player.setPosition(context.world.map().nearestFloor(
        generator.position().add(Vec2Props::of(0, 34))
      ));

      context.camera.snapToPlayer(context.world, context.player);
      generator.cancelPlannedIncidents();
      generator.setProgress(0.93F);
      generator.skillCheck().postpone(100);
    }

    context.input.simulateHold(
      InputActionEnum::interact,
      !generator.isOnline()
    );
    if(!tick.should_log) return;

    logLine(
      std::format(
        "power: state={} lit={:.2f} burning_here={} charging={}",
        nameOf(generator.state()),
        context.world.power().litAmount(generator.roomIndex()),
        context.world.isBurningLightAt(context.player.position()),
        context.player.flashlight().battery().isCharging()
      )
    );
  }
} // namespace wheel_of_fear
