#include "mods/director/demos/GrenadeDemoScenario.hpp"

#include <algorithm>
#include <format>

#include "core/input/InputController.hpp"
#include "mods/camera/CameraSystem.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/throwables/ThrowableSystem.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  void GrenadeDemoScenario::run(
    GameContextDto    &context,
    const DemoTickDto &tick
  ) {
    context.mobs.setGraceSeconds(1000);
    context.player.flashlight().setEnabled(false);
    aimAtWorldOffset(context, Vec2Props::of(90, 0));

    if(step() == 0) {
      setStep(1);
      context.mobs.clear();
    }

    if(step() == 1 && tick.seconds > 1.2F) {
      setStep(2);

      const Vec2Props target = context.camera.mouseWorldPosition(context.input);

      for(int index = 0; index < 3; ++index) {
        const auto order = static_cast<float>(index);

        context.mobs.spawn(
          MobKindEnum::crawler,
          context.world.map().nearestFloor(
            target.add(Vec2Props::of(-10 + (order * 10), -8 + (order * 8)))
          )
        );
      }

      context.input.simulatePress(InputActionEnum::throw_grenade);
    }
    if(!tick.should_log) return;

    const auto neon = context.world.map().fluids().neonSeconds();

    const auto neon_cells =
      std::count_if(neon.begin(), neon.end(), [](float value) {
        return value > 0;
      });

    logLine(
      std::format(
        "grenade: mobs_alive={} neon_cells={} puddles={}",
        context.mobs.all().size(),
        neon_cells,
        context.throwables.puddleCount()
      )
    );
  }
} // namespace wheel_of_fear
