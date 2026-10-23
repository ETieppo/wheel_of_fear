#include "mods/distractions/Distractions.hpp"

#include <algorithm>

#include "core/audio/SoundPlayer.hpp"
#include "core/time/GameClock.hpp"
#include "mods/distractions/DistractionCatalog.hpp"
#include "mods/effects/Effects.hpp"
#include "mods/lighting/Lighting.hpp"
#include "mods/mobs/Mob.hpp"
#include "mods/mobs/Mobs.hpp"
#include "mods/noise/Noise.hpp"
#include "mods/player/Player.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {

void Distractions::update(GameContextDto& context) {
    if (!context.clock.isRunning()) return;
    const auto& mobs = context.mobs.all();
    for (std::size_t index = 0; index < mobs.size(); ++index) {
        Mob& mob = *mobs[index];
        if (!mob.isMoving() || mob.isBusy() || !mob.traits().is_hostile || mob.incidentCooldown() > 0) continue;
        if (checkSceneryContact(context, mob)) continue;
        checkMobFight(context, mob, index + 1);
    }
    advanceFlashes(context);
}

bool Distractions::checkSceneryContact(GameContextDto& context, Mob& mob) {
    for (const auto& item : context.world.scenery()) {
        if (item->isBroken() || item->position().distance(mob.position()) > scenery_contact_distance) continue;
        const auto kind = item->bumpDistraction();
        if (!kind) continue;
        mob.setIncidentCooldown(mob_incident_cooldown_seconds);
        item->bump(context.world.random());
        provoke(context, *kind, item->position());
        return true;
    }
    return false;
}

void Distractions::checkMobFight(GameContextDto& context, Mob& mob, std::size_t first_other_index) {
    const auto& mobs = context.mobs.all();
    for (std::size_t index = first_other_index; index < mobs.size(); ++index) {
        Mob& other = *mobs[index];
        if (other.kind() == mob.kind() || other.isFrozen() || other.isBusy() || !other.traits().is_hostile || other.incidentCooldown() > 0 || other.isDisguised()) continue;
        const Vec2 offset = other.position().sub(mob.position());
        if (offset.length() > fight_contact_distance) continue;
        mob.setIncidentCooldown(mob_incident_cooldown_seconds);
        other.setIncidentCooldown(mob_incident_cooldown_seconds);
        const Vec2 push = offset.normalized().scale(fight_push_speed * 0.1F);
        other.slide(context.world.map(), push);
        mob.slide(context.world.map(), push.scale(-1));
        provoke(context, DistractionKindEnum::mob_fight, mob.position().lerp(other.position(), 0.5F));
        return;
    }
}

void Distractions::provoke(GameContextDto& context, DistractionKindEnum kind, Vec2 position) {
    const DistractionTraitsProps traits = DistractionCatalog::traitsOf(kind);
    active_.emplace_back(kind, position);
    context.sound_player.playAt(traits.sound, traits.volume, context.player.position(), position, traits.pitch * context.world.random().range(0.85F, 1.1F));
    context.noise.emit(position, traits.noise_radius, NoiseSourceEnum::mob_incident);
    if (traits.spark_count > 0 && traits.flash_color) context.effects.emitSparks(context.world, position, traits.spark_count, *traits.flash_color);
}

void Distractions::advanceFlashes(GameContextDto& context) {
    for (Distraction& distraction : active_) {
        distraction.advance(context.clock.deltaSeconds());
        distraction.emitFlash(context.lighting);
    }
    std::erase_if(active_, [](const Distraction& distraction) { return distraction.isFinished(); });
}

}
