#include "mods/throwables/projectiles/LureBeaconProjectile.hpp"

#include "core/audio/SoundPlayer.hpp"
#include "core/theme/Palette.hpp"
#include "mods/effects/Effects.hpp"
#include "mods/noise/Noise.hpp"
#include "mods/player/Player.hpp"
#include "mods/throwables/Throwables.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {

void LureBeaconProjectile::land(GameContextDto& context, Vec2 landing) {
    context.noise.emit(landing, impact_noise_radius, NoiseSourceEnum::throwable_impact);
    context.sound_player.playAt(SoundEnum::decoy_impact, 0.8F, context.player.position(), landing, context.world.random().range(0.9F, 1.1F));
    context.effects.emitSparks(context.world, landing, 5, Palette::neon_lime);
    context.throwables.deployBeacon(landing);
}

}
