#include "mods/mobs/species/Melter.hpp"

#include <algorithm>

#include "core/time/GameClock.hpp"
#include "mods/art/Art.hpp"
#include "mods/effects/Effects.hpp"
#include "mods/lighting/Lighting.hpp"
#include "mods/noise/Noise.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {
  void Melter::emitLights(
    GameContextDto &context
  ) {
    flash_seconds_ =
      std::max(0.0F, flash_seconds_ - context.clock.deltaSeconds());
    if(flash_seconds_ <= 0) return;
    LightProps flash;
    flash.position  = position();
    flash.radius    = 70;
    flash.intensity = 1.4F * flash_seconds_ / dissolve_flash_seconds;
    flash.color     = Color::hex(0x9cff5a);
    context.lighting.add(flash);
  }

  void Melter::think(
    GameContextDto &context
  ) {
    if(!canAct()) return;
    if(isHunting()) {
      const Vec2 direction = directionTo(destination());
      const Vec2 contact_point =
        position().add(direction.scale(species_traits.collision_radius + 6));
      const TileCoordinateProps tile = TileMap::tileAt(contact_point);
      const TileMap            &map  = context.world.map();
      if(map.isWall(tile.x, tile.y) && !map.isStructural(tile.x, tile.y)) {
        dissolveTowards(context, tile, contact_point);
        return;
      }
    }
    thinkAsHunter(context);
    if(isMoving()) leaveAcidTrail(context);
  }

  void Melter::leaveAcidTrail(
    GameContextDto &context
  ) {
    trail_seconds_ -= context.clock.deltaSeconds();
    if(trail_seconds_ > 0) return;
    trail_seconds_ = acid_trail_interval;
    context.world.map().spillAcid(position(), acid_trail_seconds);
  }

  void Melter::dissolveTowards(
    GameContextDto     &context,
    TileCoordinateProps tile,
    Vec2                contact_point
  ) {
    World      &world = context.world;
    const float delta = context.clock.deltaSeconds();
    dissolve_seconds_ += delta;
    faceToward(contact_point);
    if(world.random().chance(delta * 6))
      context.effects.emitAcidDrip(world, contact_point);
    if(dissolve_seconds_ < seconds_to_dissolve_wall) return;
    dissolve_seconds_ = 0;
    if(!world.map().dissolveWall(tile.x, tile.y)) return;
    world.map().spillAcid(
      TileMap::tileCenter(tile.x, tile.y),
      acid_trail_seconds
    );
    flash_seconds_ = dissolve_flash_seconds;
    playSound(context, SoundEnum::acid_sizzle, 0.9F, 1);
    context.noise.emit(
      contact_point,
      dissolve_noise_radius,
      NoiseSourceEnum::mob_incident
    );
  }

  void Melter::onContact(
    GameContextDto &context
  ) {
    strikeAndRetreat(context, strike_retreat_seconds);
  }

  void Melter::draw(
    GameContextDto &context
  ) const {
    const SpriteSheetProps &sprites    = context.art.sprites();
    const bool              is_working = isMoving() || dissolve_seconds_ > 0;
    const std::size_t       frame =
      animationFrame(is_working ? 9.0F : 2.0F, sprites.melter_crawl.size());
    drawBody(
      context,
      sprites.melter_crawl[frame],
      sprites.melter_crawl_glow[frame],
      MobLookDto {
        .glow_color    = Color::hex(0x9cff5a),
        .glow_strength = 1 + flash_seconds_ * 3
      }
    );
  }
} // namespace wheel_of_fear
