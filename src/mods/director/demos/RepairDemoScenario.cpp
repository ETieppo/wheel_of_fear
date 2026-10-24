#include "mods/director/demos/RepairDemoScenario.hpp"

#include <format>

#include "core/input/InputController.hpp"
#include "mods/camera/CameraSystem.hpp"
#include "mods/generators/GeneratorSystem.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  void RepairDemoScenario::run(
    GameContextDto    &context,
    const DemoTickDto &tick
  ) {
    context.mobs.setGraceSeconds(1000);
    if(context.generators.all().empty()) return;

    GeneratorEntity &generator = context.generators.all().front();

    if(step() == 0) {
      setStep(1);

      context.player.setPosition(context.world.map().nearestFloor(
        generator.position().add(Vec2Props::of(0, 30))
      ));

      context.camera.snapToPlayer(context.world, context.player);
      generator.cancelPlannedIncidents();
    }

    context.input.simulateHold(InputActionEnum::interact, tick.seconds < 40);

    const SkillCheckMinigame &check = generator.skillCheck();

    if(
      check.isActive() &&
      !check.isWarning() &&
      check.needleAngle() >=
      check.perfectStart() +
      (check.perfectWidth() * 0.5F) &&
      tick.seconds < 12
    ) {
      context.input.simulatePress(InputActionEnum::hold_breath);
    }

    if(tick.should_log) {
      logLine(
        std::format(
          "repair: state={} progress={:.3f} check_active={} last={}",
          nameOf(generator.state()),
          generator.progress(),
          check.isActive(),
          nameOf(check.lastResult())
        )
      );
    }
  }
} // namespace wheel_of_fear
