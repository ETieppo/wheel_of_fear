#include "mods/generators/Generators.hpp"

#include <algorithm>
#include <string_view>

#include "core/render/RenderConfig.hpp"
#include "core/render/Renderer.hpp"
#include "core/time/GameClock.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {
  namespace {
    constexpr std::string_view generator_labels = "ABCDEFGHIJ";
  }

  void Generators::generatePhase(
    World &world
  ) {
    generators_.clear();
    fabricators_.clear();
    exit_door_.reset(world.exitPosition());
    overload_cooldown_ = first_overload_seconds;
    std::vector<bool> occupied_rooms(world.map().rooms().size(), false);
    occupied_rooms[world.startRoomIndex()] = true;
    occupied_rooms[world.exitRoomIndex()]  = true;
    placeGenerators(world, occupied_rooms);
    placeFabricators(world, occupied_rooms);
  }

  void Generators::placeGenerators(
    World             &world,
    std::vector<bool> &occupied_rooms
  ) {
    RandomGenerator  &random = world.random();
    const std::size_t generator_count =
      std::min<std::size_t>(2 + world.phase(), 6);
    const float incident_chance =
      std::min(0.35F + 0.1F * static_cast<float>(world.phase()), 0.85F);
    for(
      std::size_t generator_index = 0; generator_index < generator_count;
      ++generator_index
    ) {
      const auto room_index = pickDistantRoom(world, occupied_rooms);
      if(!room_index) break;
      occupied_rooms[*room_index] = true;
      const RoomProps &room       = world.map().rooms()[*room_index];
      const int        corner_x   = room.centerTileX() - 1;
      const int        corner_y   = room.centerTileY() - 1;
      if(!world.map().reserveProp(corner_x, corner_y, 2, 2)) continue;
      Generator::PlannedIncidents incidents {
        GeneratorIncidentEnum::none,
        GeneratorIncidentEnum::none
      };
      for(GeneratorIncidentEnum &incident : incidents)
        if(random.chance(incident_chance))
          incident = random.chance(0.55F) ? GeneratorIncidentEnum::jam
                                          : GeneratorIncidentEnum::breakdown;
      if(world.phase() == 1 && generator_index == 0)
        incidents = {GeneratorIncidentEnum::jam, GeneratorIncidentEnum::none};
      const Vec2 center = Vec2::of(
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

  void Generators::placeFabricators(
    World             &world,
    std::vector<bool> &occupied_rooms
  ) {
    RandomGenerator  &random           = world.random();
    const auto       &rooms            = world.map().rooms();
    const std::size_t fabricator_count = 3 + world.phase() / 2;
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
        TileMap::tileCenter(column, room.top).add(Vec2::of(0, 2)),
        room_index,
        produced
      );
    }
  }

  std::optional<std::size_t> Generators::pickDistantRoom(
    World                   &world,
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
      for(const Generator &existing : generators_)
        nearest = std::min(
          nearest,
          static_cast<float>(
            rooms[room_index].gridDistance(rooms[existing.roomIndex()])
          )
        );
      const float score = nearest + world.random().unit() * 1.5F;
      if(score > best_score) {
        best_score = score;
        best_room  = room_index;
      }
    }
    return best_room;
  }

  bool Generators::areAllOnline() const {
    return !generators_.empty() &&
           std::all_of(
             generators_.begin(),
             generators_.end(),
             [](const Generator &generator) { return generator.isOnline(); }
           );
  }

  const Generator* Generators::activeRepair() const {
    for(const Generator &generator : generators_)
      if(
        generator.isBeingRepaired() ||
        generator.skillCheck().resultSeconds() > 0
      )
        return &generator;
    return nullptr;
  }

  void Generators::update(
    GameContextDto &context
  ) {
    if(!context.clock.isRunning()) return;
    const float seconds_to_charge =
      16.0F + 2.5F * static_cast<float>(std::min(context.world.phase(), 6U));
    std::size_t online_count = 0;
    for(Generator &generator : generators_) {
      generator.update(context, seconds_to_charge);
      if(generator.isOnline()) online_count += 1;
    }
    rollForOverload(context);
    for(Fabricator &fabricator : fabricators_) fabricator.update(context);
    if(areAllOnline()) exit_door_.unlock(context);
    exit_door_.update(context, online_count, generators_.size());
  }

  void Generators::rollForOverload(
    GameContextDto &context
  ) {
    overload_cooldown_ -= context.clock.deltaSeconds();
    if(overload_cooldown_ > 0 || exit_door_.isUnlocked()) return;
    const float phase_pressure =
      static_cast<float>(std::min(context.world.phase(), 6U)) * 10;
    const float chance_per_generator =
      context.clock.deltaSeconds() / (160.0F - phase_pressure);
    for(Generator &generator : generators_) {
      if(
        !generator.isOnline() ||
        generator.secondsOnline() <
        20 ||
        context.world.random().unit() >= chance_per_generator
      )
        continue;
      overload_cooldown_ = overload_cooldown_seconds;
      generator.overload(context);
      return;
    }
  }

  void Generators::draw(
    GameContextDto &context
  ) const {
    if(context.world.map().width() == 0) return;
    Renderer &renderer = context.renderer;
    for(const Fabricator &fabricator : fabricators_)
      if(renderer.isVisible(fabricator.position(), 24))
        fabricator.draw(context);
    for(const Generator &generator : generators_)
      if(renderer.isVisible(generator.position(), 36)) generator.draw(context);
    if(renderer.isVisible(exit_door_.position(), 40)) exit_door_.draw(context);
  }
} // namespace wheel_of_fear
