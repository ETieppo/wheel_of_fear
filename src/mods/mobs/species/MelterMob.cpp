#include "mods/mobs/species/MelterMob.hpp"

#include <algorithm>

#include "core/time/GameClock.hpp"
#include "mods/art/ArtLibrary.hpp"
#include "mods/effects/EffectSystem.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/noise/NoiseSystem.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  void MelterMob::emitLights(
    GameContextDto &context
  ) {
    flash_seconds_ =
      std::max(0.0F, flash_seconds_ - context.clock.deltaSeconds());
    if(flash_seconds_ <= 0) return;

    LightProps flash;

    flash.position  = position();
    flash.radius    = 70;
    flash.intensity = 1.4F * flash_seconds_ / dissolve_flash_seconds;
    flash.color     = ColorProps::hex(0x9cff5a);
    context.lighting.add(flash);
  }

  void MelterMob::think(
    GameContextDto &context
  ) {
    if(!canAct()) return;

    if(brain().isHunting()) {
      const Vec2Props direction = directionTo(brain().destination());

      const Vec2Props contact_point =
        position().add(direction.scale(species_traits.collision_radius + 6));

      const TileCoordinateProps tile = TileMapModel::tileAt(contact_point);
      const TileMapModel       &map  = context.world.map();

      if(map.isWall(tile.x, tile.y) && !map.isStructural(tile.x, tile.y)) {
        dissolveTowards(context, tile, contact_point);
        return;
      }
    }

    brain().hunt(context);
    if(isMoving()) leaveAcidTrail(context);
  }

  void MelterMob::leaveAcidTrail(
    GameContextDto &context
  ) {
    trail_seconds_ -= context.clock.deltaSeconds();
    if(trail_seconds_ > 0) return;

    trail_seconds_ = acid_trail_interval;
    context.world.map().fluids().spillAcid(position(), acid_trail_seconds);
  }

  void MelterMob::dissolveTowards(
    GameContextDto     &context,
    TileCoordinateProps tile,
    Vec2Props           contact_point
  ) {
    WorldSystem &world = context.world;
    const float  delta = context.clock.deltaSeconds();

    dissolve_seconds_ += delta;
    faceToward(contact_point);

    if(world.random().chance(delta * 6))
      context.effects.emitAcidDrip(world, contact_point);
    if(dissolve_seconds_ < seconds_to_dissolve_wall) return;

    dissolve_seconds_ = 0;
    if(!world.map().dissolveWall(tile.x, tile.y)) return;

    world.map().fluids().spillAcid(
      TileMapModel::tileCenter(tile.x, tile.y),
      acid_trail_seconds
    );

    flash_seconds_ = dissolve_flash_seconds;
    voice().playSound(context, SoundEnum::acid_sizzle, 0.9F, 1);

    context.noise.emit(
      contact_point,
      dissolve_noise_radius,
      NoiseSourceEnum::mob_incident
    );
  }

  void MelterMob::onContact(
    GameContextDto &context
  ) {
    brain().strikeAndRetreat(context, strike_retreat_seconds);
  }

  void MelterMob::draw(
    GameContextDto &context
  ) const {
    const SpriteSheetProps &sprites    = context.art.sprites();
    const bool              is_working = isMoving() || dissolve_seconds_ > 0;

    const std::size_t frame =
      animationFrame(is_working ? 9.0F : 2.0F, sprites.melter_crawl.size());

    appearance().drawBody(
      context,
      sprites.melter_crawl[frame],
      sprites.melter_crawl_glow[frame],
      MobLookDto {
        .glow_color    = ColorProps::hex(0x9cff5a),
        .glow_strength = 1 + (flash_seconds_ * 3),
      }
    );
  }
} // namespace wheel_of_fear
