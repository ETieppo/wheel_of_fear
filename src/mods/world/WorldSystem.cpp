#include "mods/world/WorldSystem.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/camera/CameraSystem.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/mobs/MobEntity.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/noise/NoiseSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/RoomLightingPlanner.hpp"
#include "mods/world/TileMapGenerator.hpp"
#include "mods/world/WorldFurnisher.hpp"
#include "mods/world/WorldPainter.hpp"
#include <algorithm>
#include <array>
#include <cstdlib>
#include <limits>
#include <utility>

namespace wheel_of_fear {
  namespace {
    constexpr std::size_t occupant_capacity = 96;

    std::
      array<
        int,
        2
      >
      gridSizeForPhase(std::uint32_t phase) {
      switch(std::min(phase, 6U)) {
        case 1:  return {4, 3};
        case 2:  return {4, 4};
        case 3:  return {5, 4};
        case 4:  return {5, 5};
        case 5:  return {5, 6};
        default: return {5, 7};
      }
    }
  } // namespace

  WorldSystem::WorldSystem() = default;

  void WorldSystem::generatePhase(
    std::uint32_t phase,
    std::uint64_t seed
  ) {
    phase_          = phase;
    random_         = RandomGenerator(seed);
    const auto grid = gridSizeForPhase(phase);

    TileMapGenerator(map_, random_).generate(grid[0], grid[1]);
    navigation_.invalidate();
    maze_shifter_.reset();
    scenery_.clear();
    lockers_.clear();
    vents_.clear();
    scorch_marks_.clear();
    neon_splashes_.clear();
    power_.clear();
    elapsed_seconds_ = 0;
    camera_shake_    = 0;
    chooseLandmarks();

    WorldFurnisher furnisher(*this);
    const auto    &rooms = map_.rooms();

    for(std::size_t room_index = 0; room_index < rooms.size(); ++room_index) {
      const RoomProps        room     = rooms[room_index];
      const LedGradientProps gradient = pickLedGradient();
      RoomLedStrip           strip    = RoomLedStrip::trace(map_, room_index);

      const float           perimeter = strip.length();
      RoomLightingPlanProps lighting_plan
        = RoomLightingPlanner::plan(map_, room_index, strip);

      power_.addRoom(
        gradient,
        std::move(strip),
        (perimeter * 0.125F) + random_.range(0, perimeter * 0.25F),
        std::move(lighting_plan)
      );

      furnisher.placeLockers(room, room_index);
      furnisher.furnishRoom(room, room_index, gradient.start_color);

      if(room_index != start_room_index_)
        addVent(map_.randomFloorInRoom(random_, room));
    }

    navigation_.refreshTowardPlayer(map_, player_start_);
  }

  LedGradientProps WorldSystem::pickLedGradient() {
    const auto &colors      = Palette::room_neon_colors;
    const auto  start_index = random_.index(colors.size());
    const auto  end_index
      = (start_index + 1 + random_.index(colors.size() - 1)) % colors.size();

    return LedGradientProps {
      .start_color = colors[start_index],
      .end_color   = colors[end_index],
    };
  }

  void WorldSystem::chooseLandmarks() {
    const auto &rooms     = map_.rooms();
    start_room_index_     = random_.index(rooms.size());
    int farthest_distance = -1;

    for(std::size_t room_index = 0; room_index < rooms.size(); ++room_index) {
      const int distance
        = (rooms[room_index].gridDistance(rooms[start_room_index_]) * 100)
        + random_.intInclusive(0, 50);

      if(distance > farthest_distance) {
        farthest_distance = distance;
        exit_room_index_  = room_index;
      }
    }

    const RoomProps &start_room = rooms[start_room_index_];
    player_start_               = TileMapModel::tileCenter(
      start_room.centerTileX(),
      start_room.centerTileY() + 3
    );

    const RoomProps &exit_room = rooms[exit_room_index_];
    exit_position_
      = TileMapModel::tileCenter(exit_room.centerTileX(), exit_room.top);
  }

  bool WorldSystem::isNearLandmark(
    int x,
    int y
  ) const {
    const Vec2Props point = TileMapModel::tileCenter(x, y);

    return point.distance(exit_position_)
      < 48
      || point.distance(player_start_)
      < 28;
  }

  void WorldSystem::removeDryNeonSplashes() {
    std::erase_if(neon_splashes_, [this](const NeonSplashProps &splash) {
      return std::ranges::none_of(
        splash.droplets,
        [this](const NeonDropletProps &droplet) {
          return map_.fluids().neonAt(droplet.position) > 0;
        }
      );
    });
  }

  void WorldSystem::addScorchMark(Vec2Props position) {
    const float rotation = random_.range(0, ScalarMath::tau);

    scorch_marks_.push_back(
      ScorchMarkProps {
        .position = position,
        .rotation = rotation,
        .variant  = random_.index(3),
      }
    );
  }

  bool WorldSystem::isBurningLightAt(Vec2Props point) const {
    const auto room_index = map_.roomIndexAt(point);
    return room_index && isRoomFullyLit(*room_index);
  }

  bool WorldSystem::isHazardAt(Vec2Props point) const {
    return isBurningLightAt(point) || map_.fluids().neonAt(point) > 0;
  }

  std::size_t WorldSystem::randomPatrolRoom(
    std::size_t                from_room,
    std::optional<std::size_t> toward_room
  ) {
    std::size_t best_room  = from_room;
    float       best_score = -std::numeric_limits<float>::infinity();

    for(
      std::size_t room_index = 0; room_index < map_.rooms().size(); ++room_index
    ) {
      if(room_index == from_room || isRoomFullyLit(room_index)) continue;

      const auto travel
        = static_cast<float>(map_.roomGridDistance(from_room, room_index));
      float score = random_.range(0, 3) - (travel * 0.6F);

      if(toward_room)
        score
          -= static_cast<float>(map_.roomGridDistance(room_index, *toward_room))
          * 0.9F;

      if(score > best_score) {
        best_score = score;
        best_room  = room_index;
      }
    }

    return best_room;
  }

  Vec2Props WorldSystem::conduitPoint(std::size_t room_index) const {
    return power_.conduitPoint(room_index);
  }

  void WorldSystem::shake(float amount) {
    camera_shake_ = std::max(camera_shake_, amount);
  }

  std::vector<Vec2Props>
  WorldSystem::collectOccupants(const GameContextDto &context) {
    std::vector<Vec2Props> occupants {context.player.position()};

    for(const auto &mob : context.mobs.all()) {
      if(occupants.size() >= occupant_capacity) break;
      occupants.push_back(mob->position());
    }

    return occupants;
  }

  void WorldSystem::update(GameContextDto &context) {
    power_.advance(context.clock.realDeltaSeconds());
    power_.emitLights(map_, context.lighting);

    const SceneryViewDto view {
      .real_seconds    = context.clock.realSeconds(),
      .is_room_powered = false,
    };

    for(const auto &item : scenery_) {
      SceneryViewDto item_view = view;

      item_view.is_room_powered = isRoomPowered(item->roomIndex());
      item->emitLight(context.lighting, item_view);
    }

    if(!context.clock.isRunning()) return;
    const float delta = context.clock.deltaSeconds();
    elapsed_seconds_ += delta;
    navigation_timer_ -= delta;

    if(navigation_timer_ <= 0) {
      navigation_timer_ = navigation_refresh_seconds;
      navigation_.refreshTowardPlayer(map_, context.player.position());
    }

    map_.fluids().evaporate(delta);
    removeDryNeonSplashes();

    const std::vector<Vec2Props> occupants = collectOccupants(context);
    MazeShifter::animateDoorways(map_, delta, occupants);
    shiftMaze(context, occupants);

    for(const auto &item : scenery_) item->update(delta);
    for(LockerEntity &locker : lockers_) locker.update(delta);
    camera_shake_ = std::max(0.0F, camera_shake_ - (delta * 2.5F));
  }

  void WorldSystem::shiftMaze(
    GameContextDto            &context,
    std::span<const Vec2Props> occupants
  ) {
    const Vec2Props            player_position = context.player.position();
    std::optional<std::size_t> focus_room = map_.roomIndexAt(player_position);
    if(!focus_room) focus_room = context.camera.roomIndex();
    if(!maze_shifter_.advance(
         map_,
         random_,
         phase_,
         context.clock.deltaSeconds(),
         context.camera.focus(),
         focus_room,
         occupants
       ))
      return;

    for(const auto &moved : maze_shifter_.movedDoorwayIndices()) {
      if(!moved) continue;
      const Vec2Props position = map_.doorways()[*moved].center();

      context.noise
        .emit(position, door_shift_noise_radius, NoiseSourceEnum::maze_shift);
      context.sound_player.playAt(
        SoundEnum::blast_door,
        door_shift_volume,
        player_position,
        position,
        random_.range(0.9F, 1.05F)
      );
    }

    shake(0.15F);
  }

  void WorldSystem::draw(GameContextDto &context) const {
    if(map_.width() == 0) return;
    WorldPainter(*this, context).paint();
  }
} // namespace wheel_of_fear
