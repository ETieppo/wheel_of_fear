#include "mods/throwables/Throwables.hpp"

#include <algorithm>
#include <format>

#include "core/audio/SoundPlayer.hpp"
#include "core/items/ItemCatalog.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/Art.hpp"
#include "mods/hud/Hud.hpp"
#include "mods/player/Player.hpp"
#include "mods/throwables/projectiles/LureBeaconProjectile.hpp"
#include "mods/throwables/projectiles/NeonGrenadeProjectile.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {
  Throwables::Throwables() = default;

  Throwables::~Throwables() = default;

  void Throwables::generatePhase(World &world) {
    projectiles_.clear();
    beacons_.clear();
    puddles_.clear();
    pickups_.clear();
    RandomGenerator  &random        = world.random();
    const auto       &rooms         = world.map().rooms();
    const std::size_t lure_count    = 3 + world.phase() / 2;
    const std::size_t grenade_count = 1 + world.phase() / 2;
    for(std::size_t index = 0; index < lure_count + grenade_count; ++index) {
      const RoomProps &room = rooms[random.index(rooms.size())];
      const ItemEnum   item =
        index < lure_count ? ItemEnum::lure_beacon : ItemEnum::neon_grenade;
      pickups_.emplace_back(world.map().randomFloorInRoom(random, room), item);
    }
  }

  void Throwables::launch(
    ItemEnum item,
    Vec2     from,
    Vec2     toward
  ) {
    const Vec2  offset   = toward.sub(from);
    const float distance = std::min(offset.length(), maximum_throw_distance);
    const Vec2  target   = from.add(offset.normalized().scale(distance));
    if(item == ItemEnum::neon_grenade) {
      projectiles_.push_back(
        std::make_unique<NeonGrenadeProjectile>(from, target)
      );
    } else {
      projectiles_.push_back(
        std::make_unique<LureBeaconProjectile>(from, target)
      );
    }
  }

  void Throwables::update(GameContextDto &context) {
    emitLights(context);
    if(!context.clock.isRunning()) return;
    advanceProjectiles(context);
    advanceBeacons(context);
    advancePuddles(context.clock.deltaSeconds());
    collectPickups(context);
  }

  void Throwables::advanceProjectiles(GameContextDto &context) {
    const TileMap &map   = context.world.map();
    std::size_t    index = 0;
    while(index < projectiles_.size()) {
      Projectile &projectile = *projectiles_[index];
      projectile.advance(context.clock.deltaSeconds());
      if(!map.isSolidAt(projectile.groundPosition()) &&
         !projectile.hasLanded()) {
        index += 1;
        continue;
      }
      const Vec2 landing = map.slide(
        projectile.start(),
        projectile.groundPosition().sub(projectile.start()),
        2
      );
      std::unique_ptr<Projectile> landed = std::move(projectiles_[index]);
      projectiles_[index]                = std::move(projectiles_.back());
      projectiles_.pop_back();
      landed->land(context, landing);
    }
  }

  void Throwables::advanceBeacons(GameContextDto &context) {
    for(LureBeacon &beacon : beacons_) beacon.update(context);
    std::erase_if(beacons_, [](const LureBeacon &beacon) {
      return beacon.isExpired();
    });
  }

  void Throwables::advancePuddles(float delta_seconds) {
    for(NeonPuddle &puddle : puddles_) puddle.update(delta_seconds);
    std::erase_if(puddles_, [](const NeonPuddle &puddle) {
      return puddle.isDry();
    });
  }

  void Throwables::emitLights(GameContextDto &context) const {
    for(const NeonPuddle &puddle : puddles_)
      puddle.emitLights(context.lighting, context.clock.realSeconds());
    for(const LureBeacon &beacon : beacons_) beacon.emitLight(context.lighting);
  }

  void Throwables::collectPickups(GameContextDto &context) {
    Player &player = context.player;
    if(!player.canBeTargeted()) return;
    const TileMap &map   = context.world.map();
    std::size_t    index = 0;
    while(index < pickups_.size()) {
      Pickup &pickup = pickups_[index];
      if(map.isSolidAt(pickup.position()))
        pickup.settleOn(map.nearestFloor(pickup.position()));
      const ItemEnum item = pickup.item();
      if(pickup.position().distance(player.position()) >=
         Pickup::reach ||
         !player.inventory().receive(item)) {
        index += 1;
        continue;
      }
      pickups_[index] = pickups_.back();
      pickups_.pop_back();
      const Item &details = ItemCatalog::of(item);
      context.sound_player.play(SoundEnum::part_pickup, 0.5F, 0, 1.4F);
      context.hud.announce(
        details.color(),
        std::format(
          "PICKED UP  {}  ({}/{})",
          details.displayName(),
          player.inventory().count(item),
          details.carryLimit()
        )
      );
    }
  }

  void Throwables::draw(GameContextDto &context) const {
    Renderer               &renderer = context.renderer;
    const SpriteSheetProps &sprites  = context.art.sprites();
    for(std::size_t index = 0; index < pickups_.size(); ++index)
      if(renderer.isVisible(pickups_[index].position(), 12))
        pickups_[index].draw(
          renderer, sprites, index, context.clock.realSeconds()
        );
    for(const LureBeacon &beacon : beacons_)
      if(renderer.isVisible(beacon.position(), 40))
        beacon.draw(renderer, sprites);
    for(const auto &projectile : projectiles_)
      projectile->draw(renderer, sprites);
  }
} // namespace wheel_of_fear
