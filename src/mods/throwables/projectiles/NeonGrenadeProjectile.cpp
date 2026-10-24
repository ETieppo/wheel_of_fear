#include "mods/throwables/projectiles/NeonGrenadeProjectile.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/theme/Palette.hpp"
#include "mods/effects/EffectSystem.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/noise/NoiseSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/throwables/NeonPuddleHazard.hpp"
#include "mods/throwables/ThrowableSystem.hpp"
#include "mods/world/WorldSystem.hpp"
#include <array>
#include <vector>

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

    context.effects.emitNeonBurst(world, landing);
    world.shake(
      ScalarMath::clamp(
        0.8F - (landing.distance(player_position) / 300),
        0.1F,
        0.6F
      )
    );

    context.mobs.explosions().destroyInBlast(context, landing, kill_radius);
    floodNeon(world, landing, NeonPuddleHazard::lifetime_seconds);
    context.throwables.spillPuddle(landing);
  }

  void NeonGrenadeProjectile::floodNeon(
    WorldSystem &world,
    Vec2Props    origin,
    float        lifetime_seconds
  ) {
    constexpr std::array<TileCoordinateProps, 4> steps {
      {
       {.x = 1, .y = 0},
       {.x = -1, .y = 0},
       {.x = 0, .y = 1},
       {.x = 0, .y = -1},
       },
    };

    TileMapModel                    &map    = world.map();
    RandomGenerator                 &random = world.random();
    const TileCoordinateProps        start  = TileMapModel::tileAt(origin);
    std::vector<TileCoordinateProps> queue {start};

    map.fluids()
      .spillNeon(start.x, start.y, lifetime_seconds + random.range(0, 6));

    for(std::size_t head = 0; head < queue.size(); ++head) {
      const TileCoordinateProps current = queue[head];

      for(const TileCoordinateProps &step : steps) {
        const int x = current.x + step.x;
        const int y = current.y + step.y;
        if(map.isWall(x, y) || map.fluids().neonAtTile(x, y) > 0) continue;

        const Vec2Props offset = TileMapModel::tileCenter(x, y).sub(
          TileMapModel::tileCenter(start.x, start.y)
        );

        const float reach = spread_tiles
          * RenderConfig::tile_size
          * (0.75F + (0.35F * random.unit()));
        if(offset.length() > reach || queue.size() >= flood_capacity) continue;

        map.fluids().spillNeon(x, y, lifetime_seconds + random.range(-4, 6));
        queue.push_back({.x = x, .y = y});
      }
    }
  }

  void NeonGrenadeProjectile::drawTrail(Renderer &renderer) const {
    renderer.drawGlowDisc(
      airPosition(),
      5,
      Palette::neon_magenta.withAlpha(0.6F),
      1.4F
    );
  }
} // namespace wheel_of_fear
