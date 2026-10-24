#include "mods/mobs/Mobs.hpp"

#include <algorithm>
#include <limits>

#include "core/audio/SoundPlayer.hpp"
#include "core/math/Scalar.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/effects/Effects.hpp"
#include "mods/lighting/Lighting.hpp"
#include "mods/mobs/MobCatalog.hpp"
#include "mods/noise/Noise.hpp"
#include "mods/player/Player.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {
  namespace {
    constexpr float freeze_threshold            = 0.035F;
    constexpr float visibility_gain             = 2.4F;
    constexpr float hug_reach                   = 22;
    constexpr float explosion_radius            = 52;
    constexpr float explosion_player_radius     = 44;
    constexpr float explosion_noise_radius      = 520;
    constexpr int   minimum_spawn_grid_distance = 2;
    constexpr float minimum_spawn_view_distance = 280;
    constexpr float danger_distance             = 170;
    constexpr float phase_grace_seconds         = 8;
    constexpr float witness_distance            = 260;
    constexpr float chasing_witness_distance    = 340;

    float spawnIntervalSeconds(
      std::uint32_t phase
    ) {
      return std::max(10.0F, 28 - static_cast<float>(phase) * 2.5F);
    }

    std::size_t populationCap(
      std::uint32_t phase
    ) {
      return 5 + 3 * static_cast<std::size_t>(std::min(phase, 8U));
    }
  } // namespace

  Mobs::Mobs() = default;

  Mobs::~Mobs() = default;

  void Mobs::populatePhase(
    World &world
  ) {
    mobs_.clear();
    spawn_timer_                    = spawnIntervalSeconds(world.phase());
    grace_seconds_                  = phase_grace_seconds;
    danger_level_                   = 0;
    RandomGenerator         &random = world.random();
    const auto              &rooms  = world.map().rooms();
    std::vector<std::size_t> distant_rooms;
    for(std::size_t room_index = 0; room_index < rooms.size(); ++room_index)
      if(
        world.map().roomGridDistance(room_index, world.startRoomIndex()) >=
        minimum_spawn_grid_distance
      )
        distant_rooms.push_back(room_index);
    if(distant_rooms.empty()) {
      for(std::size_t room_index = 0; room_index < rooms.size(); ++room_index)
        if(room_index != world.startRoomIndex())
          distant_rooms.push_back(room_index);
    }
    const std::uint32_t hostile_count = std::min(2 + world.phase() * 2, 16U);
    for(std::uint32_t index = 0; index < hostile_count; ++index) {
      const auto kind = pickSpawnKind(world);
      if(!kind) break;
      const RoomProps &room =
        rooms[distant_rooms[random.index(distant_rooms.size())]];
      spawn(*kind, world.map().randomFloorInRoom(random, room));
    }
    if(world.phase() >= MobCatalog::traitsOf(MobKindEnum::mimic).first_phase) {
      const int mimic_count = world.phase() >= 7 ? 2 : 1;
      for(int index = 0; index < mimic_count; ++index) {
        const RoomProps &room =
          rooms[distant_rooms[random.index(distant_rooms.size())]];
        const int column =
          room.left + 2 + random.intInclusive(0, room.width - 5);
        if(!world.map().reserveProp(column, room.top, 1, 1)) continue;
        spawn(
          MobKindEnum::mimic,
          TileMap::tileCenter(column, room.top).add(Vec2::of(0, 4))
        );
      }
    }
    const float moth_chance = world.phase() == 1 ? 0.7F : 0.3F;
    if(random.chance(moth_chance)) {
      const std::size_t room_index = random.index(rooms.size());
      spawn(
        MobKindEnum::moth,
        world.map().randomFloorInRoom(random, rooms[room_index])
      );
    }
  }

  std::optional<MobKindEnum> Mobs::pickSpawnKind(
    World &world
  ) {
    float total_weight = 0;
    for(const MobKindEnum kind : all_mob_kinds) {
      const MobTraitsProps &traits = MobCatalog::traitsOf(kind);
      if(traits.first_phase <= world.phase())
        total_weight += traits.spawn_weight;
    }
    if(total_weight <= 0) return std::nullopt;
    float roll = world.random().unit() * total_weight;
    for(const MobKindEnum kind : all_mob_kinds) {
      const MobTraitsProps &traits = MobCatalog::traitsOf(kind);
      if(traits.first_phase > world.phase() || traits.spawn_weight <= 0)
        continue;
      roll -= traits.spawn_weight;
      if(roll <= 0) return kind;
    }
    return MobKindEnum::crawler;
  }

  void Mobs::spawn(
    MobKindEnum kind,
    Vec2        position
  ) {
    mobs_.push_back(MobCatalog::create(kind, position));
  }

  std::vector<std::string_view> Mobs::introductionsForPhase(
    std::uint32_t phase
  ) {
    std::vector<std::string_view> introductions;
    for(const MobKindEnum kind : all_mob_kinds) {
      const MobTraitsProps &traits = MobCatalog::traitsOf(kind);
      if(traits.first_phase != phase || !traits.is_hostile) continue;
      introductions.push_back(traits.introduction);
    }
    return introductions;
  }

  bool Mobs::isHugInProgress() const {
    return std::any_of(mobs_.begin(), mobs_.end(), [](const auto &mob) {
      return mob->isBeingHugged();
    });
  }

  bool Mobs::tryStartHug(
    Vec2 position,
    Vec2 facing
  ) {
    Mob*  closest          = nullptr;
    float closest_distance = hug_reach;
    for(const auto &mob : mobs_) {
      if(mob->isBusy() || !mob->traits().is_huggable || mob->isDisguised())
        continue;
      const Vec2  offset   = mob->position().sub(position);
      const float distance = offset.length();
      if(
        distance >= closest_distance || offset.normalized().dot(facing) < -0.3F
      )
        continue;
      closest_distance = distance;
      closest          = mob.get();
    }
    if(closest == nullptr) return false;
    closest->setBeingHugged(true);
    return true;
  }

  void Mobs::cancelHug() {
    for(const auto &mob : mobs_) mob->setBeingHugged(false);
  }

  void Mobs::onGeneratorOnline(
    GameContextDto &context
  ) {
    for(const auto &mob : mobs_) mob->onGeneratorOnline(context);
  }

  void Mobs::answerShepherd(
    Vec2  caller,
    Vec2  target,
    float radius,
    float frenzy_seconds
  ) {
    for(const auto &mob : mobs_) {
      if(
        !mob->isAlive() ||
        !mob->traits().is_hostile ||
        mob->kind() ==
        MobKindEnum::shepherd ||
        mob->isDisguised()
      )
        continue;
      if(mob->distanceTo(caller) > radius) continue;
      mob->frenzy(frenzy_seconds);
      mob->hearShepherd(target);
    }
  }

  std::size_t Mobs::destroyInBlast(
    GameContextDto &context,
    Vec2            center,
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

  void Mobs::detonate(
    GameContextDto &context,
    Mob            &bomb
  ) {
    if(!bomb.isAlive()) return;
    bomb.setAlive(false);
    World     &world           = context.world;
    const Vec2 center          = bomb.position();
    const Vec2 player_position = context.player.position();
    context.effects.emitExplosion(world, center);
    context.sound_player.playAt(
      SoundEnum::explosion,
      1.0F,
      player_position,
      center,
      world.random().range(0.85F, 1.1F)
    );
    world.shake(
      Scalar::clamp(1.2F - center.distance(player_position) / 260, 0.2F, 1)
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
    if(center.distance(player_position) < explosion_player_radius)
      context.player.takeHit(context, center);
    const TileCoordinateProps center_tile = TileMap::tileAt(center);
    for(int offset_y = -1; offset_y <= 1; ++offset_y)
      for(int offset_x = -1; offset_x <= 1; ++offset_x)
        world.map().dissolveWall(
          center_tile.x + offset_x,
          center_tile.y + offset_y
        );
    for(const auto &victim : mobs_) {
      if(
        !victim->isAlive() ||
        !victim->traits().is_hostile ||
        victim->isDisguised()
      )
        continue;
      if(victim->distanceTo(center) > explosion_radius) continue;
      victim->catchBlast(context);
    }
  }

  void Mobs::update(
    GameContextDto &context
  ) {
    if(!context.clock.isRunning()) return;
    const float delta = context.clock.deltaSeconds();
    grace_seconds_    = std::max(0.0F, grace_seconds_ - delta);
    for(const auto &mob : mobs_)
      if(mob->isAlive()) mob->emitLights(context);
    const TileMap &map = context.world.map();
    for(const auto &mob : mobs_) {
      if(map.isSolidAt(mob->position()))
        mob->setPosition(map.nearestFloor(mob->position()));
      const bool was_frozen = mob->isFrozen();
      mob->setFrozen(isHeldByLight(*mob, context));
      if(was_frozen && !mob->isFrozen() && !mob->isBusy())
        mob->onLightReleased(context);
      mob->calmFrenzy(delta);
      if(mob->traits().is_hostile && !mob->isDisguised())
        mob->perceive(context, grace_seconds_ <= 0);
      mob->advanceBurning(context);
      mob->advanceHugAndCrumble(context);
    }
    if(const auto locker_index = context.player.hiding().hideEventLockerIndex())
      witnessHiding(context, *locker_index);
    for(std::size_t index = 0; index < mobs_.size(); ++index)
      mobs_[index]->update(context);
    std::erase_if(mobs_, [](const auto &mob) { return !mob->isAlive(); });
    updateSpawner(context);
    updateDanger(context);
  }

  bool Mobs::isHeldByLight(
    const Mob            &mob,
    const GameContextDto &context
  ) const {
    if(mob.isDisguised()) return false;
    const auto filter = mob.traits().frozen_by;
    if(!filter) return false;
    return context.lighting.freezingLightAt(
             context.world.map(),
             mob.position(),
             *filter
           ) > freeze_threshold;
  }

  void Mobs::witnessHiding(
    const GameContextDto &context,
    std::size_t           locker_index
  ) {
    const World &world           = context.world;
    const Vec2   locker_position = world.lockers()[locker_index].position();
    for(const auto &mob : mobs_) {
      if(
        mob->isBusy() ||
        !mob->traits().is_hostile ||
        !mob->traits().canSee() ||
        mob->isDisguised()
      )
        continue;
      const float distance     = mob->distanceTo(locker_position);
      const bool  was_tracking = mob->intent() ==
                                 MobIntentEnum::chasing &&
                                 distance < chasing_witness_distance;
      const bool  is_watching =
        distance < witness_distance && mob->isFacing(locker_position, 0.1F);
      if(!was_tracking && !is_watching) continue;
      if(!world.map().hasLineOfSight(mob->position(), locker_position))
        continue;
      mob->witnessHiding(locker_index);
    }
  }

  void Mobs::updateSpawner(
    GameContextDto &context
  ) {
    World &world = context.world;
    spawn_timer_ -= context.clock.deltaSeconds();
    if(spawn_timer_ > 0) return;
    spawn_timer_             = spawnIntervalSeconds(world.phase());
    const auto hostile_count = static_cast<std::size_t>(
      std::count_if(mobs_.begin(), mobs_.end(), [](const auto &mob) {
        return mob->traits().is_hostile;
      })
    );
    if(hostile_count >= populationCap(world.phase()) || world.vents().empty())
      return;
    const Vec2 player_position = context.player.position();
    const auto player_room     = world.map().roomIndexAt(player_position);
    for(int attempt = 0; attempt < 8; ++attempt) {
      const Vec2 vent =
        world.vents()[world.random().index(world.vents().size())];
      if(vent.distance(player_position) < minimum_spawn_view_distance) continue;
      const auto vent_room = world.map().roomIndexAt(vent);
      if(!vent_room || world.isRoomPowered(*vent_room)) continue;
      if(
        player_room &&
        world.map().roomGridDistance(*player_room, *vent_room) <
        minimum_spawn_grid_distance
      )
        continue;
      const auto kind = pickSpawnKind(world);
      if(!kind) return;
      spawn(*kind, vent);
      return;
    }
  }

  void Mobs::updateDanger(
    GameContextDto &context
  ) {
    float      nearest         = std::numeric_limits<float>::infinity();
    const Vec2 player_position = context.player.position();
    for(const auto &mob : mobs_) {
      if(
        !mob->traits().is_hostile ||
        !mob->canAct() ||
        !mob->isHunting() ||
        mob->isDisguised()
      )
        continue;
      nearest = std::min(nearest, mob->distanceTo(player_position));
    }
    const float delta = context.clock.deltaSeconds();
    const float target_level =
      Scalar::clamp(1 - nearest / danger_distance, 0, 1);
    danger_level_ = Scalar::approach(danger_level_, target_level, delta * 2);
    if(danger_level_ < 0.35F) return;
    heartbeat_timer_ -= delta;
    if(heartbeat_timer_ > 0) return;
    heartbeat_timer_ = 0.95F - danger_level_ * 0.45F;
    context.sound_player
      .play(SoundEnum::heartbeat, 0.4F + danger_level_ * 0.5F, 0, 1);
  }

  float Mobs::visibilityOf(
    const Mob            &mob,
    const GameContextDto &context
  ) {
    if(
      mob.isBurning() ||
      mob.isCrumbling() ||
      mob.isBeingHugged() ||
      mob.isDisguised()
    )
      return 1;
    const float light = context.lighting.visibilityAt(
      context.world.map(),
      mob.position(),
      mob.traits().visible_under
    );
    return Scalar::clamp(light * visibility_gain, 0, 1);
  }

  void Mobs::draw(
    GameContextDto &context
  ) {
    if(context.world.map().width() == 0) return;
    for(const auto &mob : mobs_) {
      if(!context.renderer.isVisible(mob->position(), 30)) continue;
      mob->setVisibility(visibilityOf(*mob, context));
      mob->draw(context);
    }
  }
} // namespace wheel_of_fear
