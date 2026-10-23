#include "mods/world/World.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <limits>

#include "core/audio/SoundPlayer.hpp"
#include "core/math/Scalar.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/camera/Camera.hpp"
#include "mods/lighting/Lighting.hpp"
#include "mods/mobs/Mob.hpp"
#include "mods/mobs/Mobs.hpp"
#include "mods/noise/Noise.hpp"
#include "mods/player/Player.hpp"
#include "mods/world/WorldFurnisher.hpp"
#include "mods/world/WorldPainter.hpp"

namespace wheel_of_fear {

namespace {

constexpr std::size_t occupant_capacity = 96;

std::array<int, 2> gridSizeForPhase(std::uint32_t phase) {
    switch (std::min(phase, 6U)) {
    case 1: return {4, 3};
    case 2: return {4, 4};
    case 3: return {5, 4};
    case 4: return {5, 5};
    case 5: return {5, 6};
    default: return {5, 7};
    }
}

}

World::World() = default;

void World::generatePhase(std::uint32_t phase, std::uint64_t seed) {
    phase_ = phase;
    random_ = RandomGenerator(seed);
    const auto grid = gridSizeForPhase(phase);
    map_.generate(random_, grid[0], grid[1]);
    navigation_.invalidate();
    maze_shifter_.reset();
    scenery_.clear();
    lockers_.clear();
    vents_.clear();
    scorch_marks_.clear();
    power_.clear();
    elapsed_seconds_ = 0;
    camera_shake_ = 0;
    chooseLandmarks();

    WorldFurnisher furnisher(*this);
    const auto& rooms = map_.rooms();
    for (std::size_t room_index = 0; room_index < rooms.size(); ++room_index) {
        const RoomProps room = rooms[room_index];
        const Color color = Palette::room_neon_colors[random_.index(Palette::room_neon_colors.size())];
        const float perimeter = RoomPowerGrid::perimeterOf(room);
        power_.addRoom(color, perimeter * 0.125F + random_.range(0, perimeter * 0.25F));
        furnisher.placeLockers(room, room_index);
        furnisher.furnishRoom(room, room_index, color);
        furnisher.placeNeonSigns(room, room_index);
        if (room_index != start_room_index_) addVent(map_.randomFloorInRoom(random_, room));
    }
    navigation_.refreshTowardPlayer(map_, player_start_);
}

void World::chooseLandmarks() {
    const auto& rooms = map_.rooms();
    start_room_index_ = random_.index(rooms.size());
    int farthest_distance = -1;
    for (std::size_t room_index = 0; room_index < rooms.size(); ++room_index) {
        const int distance = rooms[room_index].gridDistance(rooms[start_room_index_]) * 100 + random_.intInclusive(0, 50);
        if (distance > farthest_distance) {
            farthest_distance = distance;
            exit_room_index_ = room_index;
        }
    }
    const RoomProps& start_room = rooms[start_room_index_];
    player_start_ = TileMap::tileCenter(start_room.centerTileX(), start_room.centerTileY() + 3);
    const RoomProps& exit_room = rooms[exit_room_index_];
    exit_position_ = TileMap::tileCenter(exit_room.centerTileX(), exit_room.top);
}

bool World::isNearLandmark(int x, int y) const {
    const Vec2 point = TileMap::tileCenter(x, y);
    return point.distance(exit_position_) < 48 || point.distance(player_start_) < 28;
}

void World::addScorchMark(Vec2 position) {
    const float rotation = random_.range(0, Scalar::tau);
    scorch_marks_.push_back(ScorchMarkProps{position, rotation, random_.index(3)});
}

bool World::isBurningLightAt(Vec2 point) const {
    const auto room_index = map_.roomIndexAt(point);
    return room_index && isRoomFullyLit(*room_index);
}

bool World::isHazardAt(Vec2 point) const {
    return isBurningLightAt(point) || map_.neonAt(point) > 0;
}

std::size_t World::randomPatrolRoom(std::size_t from_room, std::optional<std::size_t> toward_room) {
    std::size_t best_room = from_room;
    float best_score = -std::numeric_limits<float>::infinity();
    for (std::size_t room_index = 0; room_index < map_.rooms().size(); ++room_index) {
        if (room_index == from_room || isRoomFullyLit(room_index)) continue;
        const auto travel = static_cast<float>(map_.roomGridDistance(from_room, room_index));
        float score = random_.range(0, 3) - travel * 0.6F;
        if (toward_room) score -= static_cast<float>(map_.roomGridDistance(room_index, *toward_room)) * 0.9F;
        if (score > best_score) {
            best_score = score;
            best_room = room_index;
        }
    }
    return best_room;
}

Vec2 World::conduitPoint(std::size_t room_index) const {
    return power_.conduitPoint(map_.rooms()[room_index], room_index);
}

void World::shake(float amount) {
    camera_shake_ = std::max(camera_shake_, amount);
}

std::vector<Vec2> World::collectOccupants(const GameContextDto& context) const {
    std::vector<Vec2> occupants{context.player.position()};
    for (const auto& mob : context.mobs.all()) {
        if (occupants.size() >= occupant_capacity) break;
        occupants.push_back(mob->position());
    }
    return occupants;
}

void World::update(GameContextDto& context) {
    power_.advance(context.clock.realDeltaSeconds());
    power_.emitLights(map_, context.lighting);
    const SceneryViewDto view{context.clock.realSeconds(), false};
    for (const auto& item : scenery_) {
        SceneryViewDto item_view = view;
        item_view.is_room_powered = isRoomPowered(item->roomIndex());
        item->emitLight(context.lighting, item_view);
    }
    if (!context.clock.isRunning()) return;
    const float delta = context.clock.deltaSeconds();
    elapsed_seconds_ += delta;
    navigation_timer_ -= delta;
    if (navigation_timer_ <= 0) {
        navigation_timer_ = navigation_refresh_seconds;
        navigation_.refreshTowardPlayer(map_, context.player.position());
    }
    map_.evaporate(delta);
    const std::vector<Vec2> occupants = collectOccupants(context);
    maze_shifter_.animateDoorways(map_, delta, occupants);
    shiftMaze(context, occupants);
    for (const auto& item : scenery_) item->update(delta);
    for (Locker& locker : lockers_) locker.update(delta);
    camera_shake_ = std::max(0.0F, camera_shake_ - delta * 2.5F);
}

void World::shiftMaze(GameContextDto& context, std::span<const Vec2> occupants) {
    const Vec2 player_position = context.player.position();
    std::optional<std::size_t> focus_room = map_.roomIndexAt(player_position);
    if (!focus_room) focus_room = context.camera.roomIndex();
    if (!maze_shifter_.advance(map_, random_, phase_, context.clock.deltaSeconds(), context.camera.focus(), focus_room, occupants)) return;
    for (const auto& moved : maze_shifter_.movedDoorwayIndices()) {
        if (!moved) continue;
        const Vec2 position = map_.doorways()[*moved].center();
        context.noise.emit(position, door_shift_noise_radius, NoiseSourceEnum::maze_shift);
        context.sound_player.playAt(SoundEnum::blast_door, door_shift_volume, player_position, position, random_.range(0.9F, 1.05F));
    }
    shake(0.15F);
}

void World::draw(GameContextDto& context) const {
    if (map_.width() == 0) return;
    WorldPainter(*this, context).paint();
}

}
