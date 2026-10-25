#include "mods/director/demos/EndingDemoScenario.hpp"
#include "core/input/InputController.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include <format>

namespace wheel_of_fear {
  void EndingDemoScenario::run(
    GameContextDto    &context,
    const DemoTickDto &tick
  ) {
    context.input.simulateHold(
      InputActionEnum::move_up,
      tick.seconds < walk_seconds
    );

    if(!tick.should_log) return;
    const Vec2Props position = context.player.position();

    logLine(
      std::format(
        "ending: t={:.1f} outdoors={} player=({:.0f}, {:.0f})",
        tick.seconds,
        context.world.isOutdoors(),
        position.x,
        position.y
      )
    );
  }
} // namespace wheel_of_fear
