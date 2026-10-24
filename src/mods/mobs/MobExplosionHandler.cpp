#include "mods/mobs/MobExplosionHandler.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/theme/Palette.hpp"
#include "mods/effects/EffectSystem.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/noise/NoiseSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  std::size_t MobExplosionHandler::destroyInBlast(
    GameContextDto &context,
    Vec2Props       center,
    float           radius
  ) {
    std::size_t destroyed = 0;

    for(const auto &victim : mobs_) {
      if(!victim->isAlive() || !victim->traits().is_hostile || victim->isBusy())
        continue;
      if(victim->distanceTo(center) > radius) continue;

      destroyed += 1;
      victim->ignite(context);
    }

    return destroyed;
  }

  void MobExplosionHandler::detonate(
    GameContextDto &context,
    MobEntity      &bomb
  ) {
    if(!bomb.isAlive()) return;
    bomb.setAlive(false);

    const Vec2Props center = bomb.position();
    announceExplosion(context, center);

    if(center.distance(context.player.position()) < explosion_player_radius)
      context.player.takeHit(context, center);

    breakSurroundingWalls(context, center);
    blastNearbyMobs(context, center);
  }

  void MobExplosionHandler::announceExplosion(
    GameContextDto &context,
    Vec2Props       center
  ) {
    WorldSystem    &world           = context.world;
    const Vec2Props player_position = context.player.position();
    context.effects.emitExplosion(world, center);
    context.sound_player.playAt(
      SoundEnum::explosion,
      1.0F,
      player_position,
      center,
      world.random().range(0.85F, 1.1F)
    );

    world.shake(
      ScalarMath::clamp(
        1.2F - (center.distance(player_position) / 260),
        0.2F,
        1
      )
    );

    context.noise
      .emit(center, explosion_noise_radius, NoiseSourceEnum::explosion);

    LightProps flash;
    flash.position          = center;
    flash.radius            = 150;
    flash.intensity         = 2.2F;
    flash.color             = Palette::neon_amber;
    flash.freezes_creatures = true;
    context.lighting.add(flash);
  }

  void MobExplosionHandler::breakSurroundingWalls(
    GameContextDto &context,
    Vec2Props       center
  ) {
    const TileCoordinateProps center_tile = TileMapModel::tileAt(center);

    for(int offset_y = -1; offset_y <= 1; ++offset_y)
      for(int offset_x = -1; offset_x <= 1; ++offset_x)
        context.world.map().dissolveWall(
          center_tile.x + offset_x,
          center_tile.y + offset_y
        );
  }

  void MobExplosionHandler::blastNearbyMobs(
    GameContextDto &context,
    Vec2Props       center
  ) {
    for(const auto &victim : mobs_) {
      if(
        !victim->isAlive()
        || !victim->traits().is_hostile
        || victim->isDisguised()
      )
        continue;

      if(victim->distanceTo(center) > explosion_radius) continue;
      victim->catchBlast(context);
    }
  }
} // namespace wheel_of_fear
