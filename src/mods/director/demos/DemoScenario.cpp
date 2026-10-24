#include "mods/director/demos/DemoScenario.hpp"
#include "core/input/InputController.hpp"
#include "core/render/RenderConfig.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include <SDL3/SDL_log.h>

namespace wheel_of_fear {
  void DemoScenario::logLine(const std::string &line) {
    SDL_Log("info(demo): %s", line.c_str());
  }

  std::string DemoScenario::optionalIndex(std::optional<std::size_t> value) {
    return value ? std::to_string(*value) : std::string("null");
  }

  void DemoScenario::spawnInSight(
    GameContextDto &context,
    MobKindEnum     kind,
    float           distance
  ) {
    const TileMapModel &map    = context.world.map();
    const Vec2Props     origin = context.player.position();

    for(int attempt = 0; attempt < 32; ++attempt) {
      const Vec2Props spot = map.nearestFloor(origin.add(
        Vec2Props::fromAngle(static_cast<float>(attempt) * 0.39F)
          .scale(distance)
      ));

      if(!map.roomIndexAt(spot) || !map.hasLineOfSight(spot, origin)) continue;
      context.mobs.spawn(kind, spot);

      return;
    }
  }

  void DemoScenario::aimAtWorldOffset(
    GameContextDto &context,
    Vec2Props       offset
  ) {
    InputController &input = context.input;

    const float pixels_per_unit
      = input.windowSize().x / static_cast<float>(RenderConfig::view_width);

    input.simulateMousePosition(
      input.windowSize().scale(0.5F).add(offset.scale(pixels_per_unit))
    );
  }
} // namespace wheel_of_fear
