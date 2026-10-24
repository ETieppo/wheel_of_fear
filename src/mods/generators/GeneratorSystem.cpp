#include "mods/generators/GeneratorSystem.hpp"

#include <algorithm>
#include <string_view>

#include "core/render/RenderConfig.hpp"
#include "core/render/Renderer.hpp"
#include "core/time/GameClock.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  namespace {
    constexpr std::string_view generator_labels = "ABCDEFGHIJ";
  }

  void GeneratorSystem::generatePhase(
    WorldSystem &world
  ) {
    generators_.clear();
    fabricators_.clear();
    exit_door_.reset(world.exitPosition());

    std::vector<bool> occupied_rooms(world.map().rooms().size(), false);

    occupied_rooms[world.startRoomIndex()] = true;
    occupied_rooms[world.exitRoomIndex()]  = true;
    placeGenerators(world, occupied_rooms);
    placeFabricators(world, occupied_rooms);
  }

  void GeneratorSystem::placeGenerators(
    WorldSystem       &world,
    std::vector<bool> &occupied_rooms
  ) {
    RandomGenerator &random = world.random();

    const std::size_t generator_count =
      std::min<std::size_t>(2 + world.phase(), 6);

    const float incident_chance =
      std::min(0.35F + (0.1F * static_cast<float>(world.phase())), 0.85F);

    for(
      std::size_t generator_index = 0; generator_index < generator_count;
      ++generator_index
    ) {
      const auto room_index = pickDistantRoom(world, occupied_rooms);
      if(!room_index) break;

      occupied_rooms[*room_index] = true;

      const RoomProps &room     = world.map().rooms()[*room_index];
      const int        corner_x = room.centerTileX() - 1;
      const int        corner_y = room.centerTileY() - 1;
      if(!world.map().reserveProp(corner_x, corner_y, 2, 2)) continue;

      GeneratorEntity::PlannedIncidents incidents {
        GeneratorIncidentEnum::none,
        GeneratorIncidentEnum::none,
      };

      for(GeneratorIncidentEnum &incident : incidents)
        if(random.chance(incident_chance))
          incident = random.chance(0.55F) ? GeneratorIncidentEnum::jam
                                          : GeneratorIncidentEnum::breakdown;

      if(world.phase() == 1 && generator_index == 0)
        incidents = {GeneratorIncidentEnum::jam, GeneratorIncidentEnum::none};

      const Vec2Props center = Vec2Props::of(
        static_cast<float>(corner_x + 1) * RenderConfig::tile_size,
        static_cast<float>(corner_y + 1) * RenderConfig::tile_size
      );

      generators_.emplace_back(
        center,
        *room_index,
        generator_labels[generators_.size()],
        incidents
      );
    }
  }

  void GeneratorSystem::placeFabricators(
    WorldSystem       &world,
    std::vector<bool> &occupied_rooms
  ) {
    RandomGenerator  &random           = world.random();
    const auto       &rooms            = world.map().rooms();
    const std::size_t fabricator_count = 3 + (world.phase() / 2);

    for(
      int attempt = 0; fabricators_.size() < fabricator_count && attempt < 300;
      ++attempt
    ) {
      const std::size_t room_index = random.index(rooms.size());
      if(room_index == world.exitRoomIndex()) continue;
      if(occupied_rooms[room_index] && attempt < 200) continue;

      const RoomProps &room = rooms[room_index];
      const int column = room.left + 3 + random.intInclusive(0, room.width - 7);
      if(!world.map().reserveProp(column, room.top, 1, 1)) continue;

      occupied_rooms[room_index] = true;

      const auto produced =
        static_cast<ItemEnum>(fabricators_.size() % item_count);

      fabricators_.emplace_back(
        TileMapModel::tileCenter(column, room.top).add(Vec2Props::of(0, 2)),
        room_index,
        produced
      );
    }
  }

  std::optional<std::size_t> GeneratorSystem::pickDistantRoom(
    WorldSystem             &world,
    const std::vector<bool> &occupied_rooms
  ) const {
    std::optional<std::size_t> best_room;
    float                      best_score = -1;
    const auto                &rooms      = world.map().rooms();

    for(std::size_t room_index = 0; room_index < rooms.size(); ++room_index) {
      if(occupied_rooms[room_index]) continue;

      auto nearest = static_cast<float>(
        rooms[room_index].gridDistance(rooms[world.startRoomIndex()])
      );

      for(const GeneratorEntity &existing : generators_)
        nearest = std::min(
          nearest,
          static_cast<float>(
            rooms[room_index].gridDistance(rooms[existing.roomIndex()])
          )
        );

      const float score = nearest + (world.random().unit() * 1.5F);

      if(score > best_score) {
        best_score = score;
        best_room  = room_index;
      }
    }

    return best_room;
  }

  bool GeneratorSystem::areAllOnline() const {
    return !generators_.empty() && std::ranges::all_of(
                                     generators_,
                                     [](const GeneratorEntity &generator) {
                                       return generator.isOnline();
                                     }
                                   );
  }

  const GeneratorEntity* GeneratorSystem::activeRepair() const {
    for(const GeneratorEntity &generator : generators_)
      if(
        generator.isBeingRepaired() ||
        generator.skillCheck().resultSeconds() > 0
      )
        return &generator;

    return nullptr;
  }

  void GeneratorSystem::update(
    GameContextDto &context
  ) {
    if(!context.clock.isRunning()) return;

    const float seconds_to_charge =
      16.0F + (2.5F * static_cast<float>(std::min(context.world.phase(), 6U)));

    std::size_t online_count = 0;

    for(GeneratorEntity &generator : generators_) {
      generator.update(context, seconds_to_charge);
      if(generator.isOnline()) online_count += 1;
    }
    for(FabricatorEntity &fabricator : fabricators_) fabricator.update(context);
    if(areAllOnline()) exit_door_.unlock(context);
    exit_door_.update(context, online_count, generators_.size());
  }

  void GeneratorSystem::draw(
    GameContextDto &context
  ) const {
    if(context.world.map().width() == 0) return;

    Renderer &renderer = context.renderer;

    for(const FabricatorEntity &fabricator : fabricators_)
      if(renderer.isVisible(fabricator.position(), 24))
        fabricator.draw(context);

    for(const GeneratorEntity &generator : generators_)
      if(renderer.isVisible(generator.position(), 36)) generator.draw(context);
    if(renderer.isVisible(exit_door_.position(), 40)) exit_door_.draw(context);
  }
} // namespace wheel_of_fear
