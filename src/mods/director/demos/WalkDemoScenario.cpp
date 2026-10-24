#include "mods/director/demos/WalkDemoScenario.hpp"

#include <format>
#include <optional>

#include "core/input/InputController.hpp"
#include "core/math/ScalarMath.hpp"
#include "mods/camera/CameraSystem.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  void WalkDemoScenario::run(
    GameContextDto    &context,
    const DemoTickDto &tick
  ) {
    WorldSystem     &world  = context.world;
    PlayerEntity    &player = context.player;
    InputController &input  = context.input;

    context.mobs.setGraceSeconds(1000);

    const std::size_t        start_room = world.startRoomIndex();
    std::optional<Vec2Props> target;

    for(const DoorwayProps &doorway : world.map().doorways()) {
      if(!doorway.connects(start_room) || !doorway.is_open) continue;

      const Vec2Props other_center =
        world.map().rooms()[doorway.otherRoom(start_room)].center();

      target = doorway.center().add(
        other_center.sub(doorway.center()).normalized().scale(90)
      );

      break;
    }

    for(
      const InputActionEnum action : {
        InputActionEnum::move_left,
        InputActionEnum::move_right,
        InputActionEnum::move_up,
        InputActionEnum::move_down,
      }
    )
      input.simulateHold(action, false);
    if(!target) return;

    const bool reached = player.distanceTo(*target) < 12;

    if(!reached && step() == 0) {
      const Vec2Props direction = world.navigation().directionToward(
        world.map(),
        player.position(),
        *target,
        world.elapsedSeconds()
      );

      const Vec2Props steer = direction.length() < 0.5F
                              ? target->sub(player.position()).normalized()
                              : direction;

      input.simulateHold(InputActionEnum::move_left, steer.x < -0.38F);
      input.simulateHold(InputActionEnum::move_right, steer.x > 0.38F);
      input.simulateHold(InputActionEnum::move_up, steer.y < -0.38F);
      input.simulateHold(InputActionEnum::move_down, steer.y > 0.38F);
    } else if(step() == 0) {
      setStep(1);
    }

    aimAtWorldOffset(
      context,
      step() == 1 && ScalarMath::floorModulo(tick.seconds, 2) < 1
        ? Vec2Props::of(200, 0)
        : Vec2Props::of(0, 0)
    );
    if(!tick.should_log) return;

    const CameraSystem &camera = context.camera;

    logLine(
      std::format(
        "walk: player_room={} camera_room={} transition={:.2f} "
        "camera_offset=({:.0f},{:.0f}) reached={}",
        optionalIndex(world.map().roomIndexAt(player.position())),
        optionalIndex(camera.roomIndex()),
        camera.transitionSeconds(),
        camera.focus().x - player.position().x,
        camera.focus().y - player.position().y,
        step() == 1
      )
    );
  }
} // namespace wheel_of_fear
