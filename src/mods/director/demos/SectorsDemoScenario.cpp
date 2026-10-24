#include "mods/director/demos/SectorsDemoScenario.hpp"
#include "mods/generators/GeneratorSystem.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include <format>

namespace wheel_of_fear {
  void SectorsDemoScenario::run(
    GameContextDto                     &context,
    [[maybe_unused]] const DemoTickDto &tick
  ) {
    WorldSystem     &world      = context.world;
    GeneratorSystem &generators = context.generators;

    if(world.phase() != last_phase_) {
      last_phase_ = world.phase();

      logLine(
        std::format(
          "sectors: entered sector {} map={}x{} rooms={} generators={} "
          "fabricators={} mobs={}",
          world.phase(),
          world.map().width(),
          world.map().height(),
          world.map().rooms().size(),
          generators.all().size(),
          generators.fabricators().size(),
          context.mobs.all().size()
        )
      );
    }

    if(!generators.areAllOnline()) {
      for(GeneratorEntity &generator : generators.all())
        generator.forceOnline(context);
      return;
    }

    if(generators.exitDoor().openAmount() > 0.7F)
      context.player.setPosition(world.exitPosition().add(Vec2Props::of(0, 8)));
  }
} // namespace wheel_of_fear
