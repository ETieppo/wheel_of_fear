#include "mods/distractions/DistractionSystem.hpp"

#include <algorithm>

#include "core/audio/SoundPlayer.hpp"
#include "core/time/GameClock.hpp"
#include "mods/distractions/DistractionCatalog.hpp"
#include "mods/effects/EffectSystem.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/mobs/MobEntity.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/noise/NoiseSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  void DistractionSystem::update(
    GameContextDto &context
  ) {
    if(!context.clock.isRunning()) return;

    const auto &mobs = context.mobs.all();

    for(std::size_t index = 0; index < mobs.size(); ++index) {
      MobEntity &mob = *mobs[index];

      if(
        !mob.isMoving() ||
        mob.isBusy() ||
        !mob.traits().is_hostile ||
        mob.incidentCooldown() > 0
      )
        continue;
      if(checkSceneryContact(context, mob)) continue;

      checkMobFight(context, mob, index + 1);
    }

    advanceFlashes(context);
  }

  bool DistractionSystem::checkSceneryContact(
    GameContextDto &context,
    MobEntity      &mob
  ) {
    for(const auto &item : context.world.scenery()) {
      if(
        item->isBroken() ||
        item->position().distance(mob.position()) > scenery_contact_distance
      )
        continue;

      const auto kind = item->bumpDistraction();
      if(!kind) continue;

      mob.setIncidentCooldown(mob_incident_cooldown_seconds);
      item->bump(context.world.random());
      provoke(context, *kind, item->position());

      return true;
    }

    return false;
  }

  void DistractionSystem::checkMobFight(
    GameContextDto &context,
    MobEntity      &mob,
    std::size_t     first_other_index
  ) {
    const auto &mobs = context.mobs.all();

    for(std::size_t index = first_other_index; index < mobs.size(); ++index) {
      MobEntity &other = *mobs[index];

      if(
        other.kind() ==
        mob.kind() ||
        other.condition().isFrozen() ||
        other.isBusy() ||
        !other.traits().is_hostile ||
        other.incidentCooldown() >
        0 ||
        other.isDisguised()
      )
        continue;

      const Vec2Props offset = other.position().sub(mob.position());
      if(offset.length() > fight_contact_distance) continue;

      mob.setIncidentCooldown(mob_incident_cooldown_seconds);
      other.setIncidentCooldown(mob_incident_cooldown_seconds);

      const Vec2Props push = offset.normalized().scale(fight_push_speed * 0.1F);

      other.slide(context.world.map(), push);
      mob.slide(context.world.map(), push.scale(-1));

      provoke(
        context,
        DistractionKindEnum::mob_fight,
        mob.position().lerp(other.position(), 0.5F)
      );

      return;
    }
  }

  void DistractionSystem::provoke(
    GameContextDto     &context,
    DistractionKindEnum kind,
    Vec2Props           position
  ) {
    const DistractionTraitsProps traits = DistractionCatalog::traitsOf(kind);

    active_.emplace_back(kind, position);

    context.sound_player.playAt(
      traits.sound,
      traits.volume,
      context.player.position(),
      position,
      traits.pitch * context.world.random().range(0.85F, 1.1F)
    );

    context.noise
      .emit(position, traits.noise_radius, NoiseSourceEnum::mob_incident);

    if(traits.spark_count > 0 && traits.flash_color)
      context.effects.emitSparks(
        context.world,
        position,
        traits.spark_count,
        *traits.flash_color
      );
  }

  void DistractionSystem::advanceFlashes(
    GameContextDto &context
  ) {
    for(DistractionEvent &distraction : active_) {
      distraction.advance(context.clock.deltaSeconds());
      distraction.emitFlash(context.lighting);
    }

    std::erase_if(active_, [](const DistractionEvent &distraction) {
      return distraction.isFinished();
    });
  }
} // namespace wheel_of_fear
