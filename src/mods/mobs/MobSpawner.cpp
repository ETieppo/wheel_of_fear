#include "mods/mobs/MobSpawner.hpp"
#include "core/time/GameClock.hpp"
#include "mods/mobs/MobCatalog.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include <algorithm>

namespace wheel_of_fear {
  std::vector<std::string_view>
  MobSpawner::introductionsForPhase(std::uint32_t phase) {
    std::vector<std::string_view> introductions;

    for(const MobKindEnum kind : all_mob_kinds) {
      const MobTraitsProps &traits = MobCatalog::traitsOf(kind);
      if(traits.first_phase != phase || !traits.is_hostile) continue;
      introductions.push_back(traits.introduction);
    }

    return introductions;
  }

  void MobSpawner::spawn(
    MobKindEnum kind,
    Vec2Props   position
  ) {
    mobs_.push_back(MobCatalog::create(kind, position));
  }

  void MobSpawner::populatePhase(WorldSystem &world) {
    spawn_timer_ = spawnIntervalSeconds(world.phase());
    const std::vector<std::size_t> distant_rooms = distantRoomsFromStart(world);

    spawnHostiles(world, distant_rooms);
    spawnMimics(world, distant_rooms);
    spawnMoth(world);
  }

  void MobSpawner::update(GameContextDto &context) {
    WorldSystem &world = context.world;

    spawn_timer_ -= context.clock.deltaSeconds();
    if(spawn_timer_ > 0) return;

    spawn_timer_ = spawnIntervalSeconds(world.phase());
    if(hostileCount() >= populationCap(world.phase()) || world.vents().empty())
      return;

    spawnFromVent(context);
  }

  std::optional<MobKindEnum> MobSpawner::pickSpawnKind(WorldSystem &world) {
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

  float MobSpawner::spawnIntervalSeconds(std::uint32_t phase) {
    return std::max(10.0F, 28 - (static_cast<float>(phase) * 2.5F));
  }

  std::size_t MobSpawner::populationCap(std::uint32_t phase) {
    return 5 + (3 * static_cast<std::size_t>(std::min(phase, 8U)));
  }

  std::vector<std::size_t>
  MobSpawner::distantRoomsFromStart(const WorldSystem &world) {
    const auto              &rooms = world.map().rooms();
    std::vector<std::size_t> distant_rooms;

    for(std::size_t room_index = 0; room_index < rooms.size(); ++room_index)
      if(
        world.map().roomGridDistance(room_index, world.startRoomIndex())
        >= minimum_spawn_grid_distance
      )
        distant_rooms.push_back(room_index);

    if(distant_rooms.empty()) {
      for(std::size_t room_index = 0; room_index < rooms.size(); ++room_index)
        if(room_index != world.startRoomIndex())
          distant_rooms.push_back(room_index);
    }

    return distant_rooms;
  }

  void MobSpawner::spawnHostiles(
    WorldSystem                    &world,
    const std::vector<std::size_t> &distant_rooms
  ) {
    RandomGenerator    &random        = world.random();
    const auto         &rooms         = world.map().rooms();
    const std::uint32_t hostile_count = std::min(2 + (world.phase() * 2), 16U);

    for(std::uint32_t index = 0; index < hostile_count; ++index) {
      const auto kind = pickSpawnKind(world);
      if(!kind) break;

      const RoomProps &room
        = rooms[distant_rooms[random.index(distant_rooms.size())]];

      spawn(*kind, world.map().randomFloorInRoom(random, room));
    }
  }

  void MobSpawner::spawnMimics(
    WorldSystem                    &world,
    const std::vector<std::size_t> &distant_rooms
  ) {
    if(world.phase() < MobCatalog::traitsOf(MobKindEnum::mimic).first_phase)
      return;

    RandomGenerator &random      = world.random();
    const auto      &rooms       = world.map().rooms();
    const int        mimic_count = world.phase() >= 7 ? 2 : 1;

    for(int index = 0; index < mimic_count; ++index) {
      for(int attempt = 0; attempt < mimic_placement_attempts; ++attempt) {
        const RoomProps &room
          = rooms[distant_rooms[random.index(distant_rooms.size())]];
        const int column
          = room.left + 2 + random.intInclusive(0, room.width - 5);

        if(!world.map().reserveProp(column, room.top, 1, 1)) continue;
        spawn(
          MobKindEnum::mimic,
          TileMapModel::tileCenter(column, room.top).add(Vec2Props::of(0, 4))
        );

        break;
      }
    }
  }

  void MobSpawner::spawnMoth(WorldSystem &world) {
    RandomGenerator &random      = world.random();
    const auto      &rooms       = world.map().rooms();
    const float      moth_chance = world.phase() == 1 ? 0.7F : 0.3F;

    if(!random.chance(moth_chance)) return;
    const std::size_t room_index = random.index(rooms.size());

    spawn(
      MobKindEnum::moth,
      world.map().randomFloorInRoom(random, rooms[room_index])
    );
  }

  std::size_t MobSpawner::hostileCount() const {
    return static_cast<std::size_t>(
      std::count_if(mobs_.begin(), mobs_.end(), [](const auto &mob) {
        return mob->traits().is_hostile;
      })
    );
  }

  void MobSpawner::spawnFromVent(GameContextDto &context) {
    WorldSystem    &world           = context.world;
    const Vec2Props player_position = context.player.position();
    const auto      player_room     = world.map().roomIndexAt(player_position);

    for(int attempt = 0; attempt < 8; ++attempt) {
      const Vec2Props vent
        = world.vents()[world.random().index(world.vents().size())];
      if(vent.distance(player_position) < minimum_spawn_view_distance) continue;

      const auto vent_room = world.map().roomIndexAt(vent);
      if(!vent_room || world.isRoomPowered(*vent_room)) continue;

      if(
        player_room
        && world.map().roomGridDistance(*player_room, *vent_room)
        < minimum_spawn_grid_distance
      )
        continue;

      const auto kind = pickSpawnKind(world);
      if(!kind) return;

      spawn(*kind, vent);
      return;
    }
  }
} // namespace wheel_of_fear
