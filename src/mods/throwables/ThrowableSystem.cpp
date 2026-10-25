#include "mods/throwables/ThrowableSystem.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/items/ItemCatalog.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/ArtLibrary.hpp"
#include "mods/hud/HudSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/throwables/projectiles/LureBeaconProjectile.hpp"
#include "mods/throwables/projectiles/NeonGrenadeProjectile.hpp"
#include "mods/world/WorldSystem.hpp"
#include <algorithm>
#include <format>

namespace wheel_of_fear {
  ThrowableSystem::ThrowableSystem()  = default;
  ThrowableSystem::~ThrowableSystem() = default;

  void ThrowableSystem::generatePhase(WorldSystem &world) {
    projectiles_.clear();
    beacons_.clear();
    puddles_.clear();
    pickups_.clear();
    grenade_color_
      = ItemCatalog::of(ItemEnum::neon_grenade).colorForPhase(world.phase());

    RandomGenerator  &random        = world.random();
    const auto       &rooms         = world.map().rooms();
    const std::size_t lure_count    = 3 + (world.phase() / 2);
    const std::size_t grenade_count = 1 + (world.phase() / 2);

    for(std::size_t index = 0; index < lure_count + grenade_count; ++index) {
      const RoomProps &room = rooms[random.index(rooms.size())];
      const ItemEnum   item
        = index < lure_count ? ItemEnum::lure_beacon : ItemEnum::neon_grenade;
      pickups_.emplace_back(world.map().randomFloorInRoom(random, room), item);
    }
  }

  void ThrowableSystem::launch(
    ItemEnum  item,
    Vec2Props from,
    Vec2Props toward
  ) {
    const Vec2Props offset = toward.sub(from);
    const float distance   = std::min(offset.length(), maximum_throw_distance);
    const Vec2Props target = from.add(offset.normalized().scale(distance));

    if(item == ItemEnum::neon_grenade) {
      projectiles_.push_back(
        std::make_unique<NeonGrenadeProjectile>(from, target, grenade_color_)
      );
    } else {
      projectiles_.push_back(
        std::make_unique<LureBeaconProjectile>(from, target)
      );
    }
  }

  void ThrowableSystem::update(GameContextDto &context) {
    emitLights(context);
    if(!context.clock.isRunning()) return;

    advanceProjectiles(context);
    advanceBeacons(context);
    advancePuddles(context.clock.deltaSeconds());
    collectPickups(context);
  }

  void ThrowableSystem::advanceProjectiles(GameContextDto &context) {
    const TileMapModel &map   = context.world.map();
    std::size_t         index = 0;

    while(index < projectiles_.size()) {
      Projectile &projectile = *projectiles_[index];
      projectile.advance(context.clock.deltaSeconds());

      if(
        !map.isSolidAt(projectile.groundPosition()) && !projectile.hasLanded()
      ) {
        index += 1;
        continue;
      }

      const Vec2Props landing = map.slide(
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

  void ThrowableSystem::advanceBeacons(GameContextDto &context) {
    for(LureBeaconEntity &beacon : beacons_) beacon.update(context);

    std::erase_if(beacons_, [](const LureBeaconEntity &beacon) {
      return beacon.isExpired();
    });
  }

  void ThrowableSystem::advancePuddles(float delta_seconds) {
    for(NeonPuddleHazard &puddle : puddles_) puddle.update(delta_seconds);

    std::erase_if(puddles_, [](const NeonPuddleHazard &puddle) {
      return puddle.isDry();
    });
  }

  void ThrowableSystem::emitLights(GameContextDto &context) const {
    for(const NeonPuddleHazard &puddle : puddles_)
      puddle.emitLights(context.lighting, context.clock.realSeconds());

    for(const LureBeaconEntity &beacon : beacons_)
      beacon.emitLight(context.lighting);
  }

  void ThrowableSystem::collectPickups(GameContextDto &context) {
    PlayerEntity &player = context.player;
    if(!player.canBeTargeted()) return;

    const TileMapModel &map   = context.world.map();
    std::size_t         index = 0;

    while(index < pickups_.size()) {
      PickupEntity &pickup = pickups_[index];

      if(map.isSolidAt(pickup.position()))
        pickup.settleOn(map.nearestFloor(pickup.position()));
      const ItemEnum item = pickup.item();

      if(
        pickup.position().distance(player.position())
        >= PickupEntity::reach
        || !player.inventory().receive(item)
      ) {
        index += 1;
        continue;
      }

      pickups_[index] = pickups_.back();
      pickups_.pop_back();

      const Item &details = ItemCatalog::of(item);
      context.sound_player.play(SoundEnum::part_pickup, 0.5F, 0, 1.4F);

      context.hud.announce(
        details.colorForPhase(context.world.phase()),
        std::format(
          "PICKED UP  {}  ({}/{})",
          details.displayName(),
          player.inventory().count(item),
          details.carryLimit()
        )
      );
    }
  }

  void ThrowableSystem::draw(GameContextDto &context) const {
    Renderer               &renderer = context.renderer;
    const SpriteSheetProps &sprites  = context.art.sprites();

    for(std::size_t index = 0; index < pickups_.size(); ++index)
      if(renderer.isVisible(pickups_[index].position(), 12))
        pickups_[index].draw(
          renderer,
          sprites,
          ItemCatalog::of(pickups_[index].item())
            .colorForPhase(context.world.phase()),
          index,
          context.clock.realSeconds()
        );

    for(const LureBeaconEntity &beacon : beacons_)
      if(renderer.isVisible(beacon.position(), 40))
        beacon.draw(renderer, sprites);

    for(const auto &projectile : projectiles_)
      projectile->draw(renderer, sprites);
  }
} // namespace wheel_of_fear
