#include "mods/director/demos/DoorsDemoScenario.hpp"
#include "mods/camera/CameraSystem.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/TileMapConnectivityAnalyzer.hpp"
#include "mods/world/WorldSystem.hpp"
#include <algorithm>
#include <format>

namespace wheel_of_fear {
  void DoorsDemoScenario::run(
    GameContextDto    &context,
    const DemoTickDto &tick
  ) {
    WorldSystem &world = context.world;
    context.mobs.setGraceSeconds(1000);
    const std::size_t start_room = world.startRoomIndex();

    if(step() == 0) {
      setStep(1);

      for(const DoorwayProps &doorway : world.map().doorways()) {
        if(!doorway.connects(start_room) || doorway.is_open) continue;
        const Vec2Props room_center = world.map().rooms()[start_room].center();

        context.player.setPosition(
          world.map().nearestFloor(doorway.center().add(
            room_center.sub(doorway.center()).normalized().scale(60)
          ))
        );

        context.camera.snapToPlayer(world, context.player);
        break;
      }
    }

    world.mazeShifter().setTimerSeconds(
      std::min(world.mazeShifter().timerSeconds(), 1.5F)
    );

    for(DoorwayProps &doorway : world.map().doorways())
      doorway.seconds_since_change
        = std::max(doorway.seconds_since_change, 11.0F);

    for(const auto &moved : world.mazeShifter().movedDoorwayIndices())
      if(moved) total_moved_doorways_ += 1;
    if(!tick.should_log) return;

    logLine(
      std::format(
        "doors: moved_total={} connected={} floor_connected={}",
        total_moved_doorways_,
        TileMapConnectivityAnalyzer::isRoomGraphConnected(world.map()),
        TileMapConnectivityAnalyzer::isEveryFloorConnected(world.map())
      )
    );
  }
} // namespace wheel_of_fear
