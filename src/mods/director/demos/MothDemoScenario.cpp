#include "mods/director/demos/MothDemoScenario.hpp"

#include <format>
#include <string_view>

#include "mods/mobs/MobSystem.hpp"
#include "mods/mobs/species/MothMob.hpp"
#include "mods/player/PlayerEntity.hpp"

namespace wheel_of_fear {
  void MothDemoScenario::run(
    GameContextDto    &context,
    const DemoTickDto &tick
  ) {
    PlayerEntity &player = context.player;

    context.mobs.setGraceSeconds(0);
    player.flashlight().setEnabled(false);

    if(step() == 0) {
      setStep(1);
      context.mobs.clear();

      context.mobs.spawn(
        MobKindEnum::moth,
        player.position().add(Vec2Props::of(8, 0))
      );

      spawnInSight(context, MobKindEnum::crawler, 160);
    }
    if(!tick.should_log) return;

    Vec2Props        moth_position {};
    std::string_view stance = "missing";

    for(const auto &mob : context.mobs.all()) {
      if(const auto* moth = dynamic_cast<const MothMob*>(mob.get())) {
        moth_position = moth->position();
        stance        = nameOf(moth->stance());
      }
    }

    for(const auto &mob : context.mobs.all()) {
      if(mob->kind() != MobKindEnum::crawler) continue;

      logLine(
        std::format(
          "moth: stance={} crawler frozen={} intent={} "
          "distance_to_moth={:.0f} distance_to_player={:.0f}",
          stance,
          mob->condition().isFrozen(),
          nameOf(mob->brain().intent()),
          mob->distanceTo(moth_position),
          mob->distanceTo(player.position())
        )
      );
    }
  }
} // namespace wheel_of_fear
