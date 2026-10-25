#include "mods/throwables/projectiles/NeonGrenadeProjectile.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/theme/Palette.hpp"
#include "mods/effects/EffectSystem.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/noise/NoiseSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/throwables/NeonPaintSpiller.hpp"
#include "mods/throwables/NeonPuddleHazard.hpp"
#include "mods/throwables/ThrowableSystem.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  void NeonGrenadeProjectile::land(
    GameContextDto &context,
    Vec2Props       landing
  ) {
    WorldSystem    &world           = context.world;
    const Vec2Props player_position = context.player.position();

    context.noise.emit(landing, noise_radius, NoiseSourceEnum::explosion);
    context.sound_player.playAt(
      SoundEnum::neon_burst,
      1.0F,
      player_position,
      landing,
      world.random().range(0.92F, 1.05F)
    );

    context.effects.emitNeonBurst(world, landing, color_);
    world.shake(
      ScalarMath::clamp(
        0.8F - (landing.distance(player_position) / 300),
        0.1F,
        0.6F
      )
    );

    context.mobs.explosions().destroyInBlast(context, landing, kill_radius);
    world.addNeonSplash(
      NeonPaintSpiller::spill(
        world,
        landing,
        spread_tiles * RenderConfig::tile_size,
        NeonPuddleHazard::lifetime_seconds,
        color_
      )
    );
    context.throwables.spillPuddle(landing);
  }

  void NeonGrenadeProjectile::drawTrail(Renderer &renderer) const {
    renderer.drawGlowDisc(airPosition(), 5, color_.withAlpha(0.6F), 1.4F);
  }
} // namespace wheel_of_fear
