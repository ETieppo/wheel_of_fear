#include "mods/world/MazeShifter.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/render/RenderConfig.hpp"
#include "mods/world/TileMapConnectivityAnalyzer.hpp"
#include <algorithm>
#include <cmath>

namespace wheel_of_fear {
  namespace {
    constexpr float       minimum_interval_seconds     = 12;
    constexpr float       maximum_interval_seconds     = 24;
    constexpr float       interval_reduction_per_phase = 1.2F;
    constexpr float       hidden_margin                = 40;
    constexpr float       door_travel_per_second       = 0.9F;
    constexpr float       recent_change_seconds        = 10;
    constexpr float       occupant_clearance           = 10;
    constexpr float       nearby_opening_reach         = 40;
    constexpr int         neighborhood_grid_distance   = 1;
    constexpr std::size_t candidate_capacity           = 64;

    float sign(float value) {
      if(value > 0) return 1;
      if(value < 0) return -1;
      return 0;
    }
  } // namespace

  void MazeShifter::reset() {
    timer_seconds_         = first_shift_delay_seconds;
    moved_doorway_indices_ = {};
  }

  void MazeShifter::animateDoorways(
    TileMapModel              &map,
    float                      delta_seconds,
    std::span<const Vec2Props> occupants
  ) {
    for(DoorwayProps &doorway : map.doorways()) {
      doorway.seconds_since_change += delta_seconds;
      const float target = doorway.is_open ? 1.0F : 0.0F;

      float next = ScalarMath::clamp(
        doorway.open_amount
          + (sign(target - doorway.open_amount) * delta_seconds * door_travel_per_second),
        0,
        1
      );

      if(
        std::abs(target - doorway.open_amount)
        <= delta_seconds
        * door_travel_per_second
      )
        next = target;

      const bool tiles_are_passable
        = !map.isWall(doorway.first_x, doorway.first_y);
      if(tiles_are_passable && next < 0.5F && isOccupied(doorway, occupants))
        next = 0.5F;

      doorway.open_amount           = next;
      const bool should_be_passable = next >= 0.5F;
      if(should_be_passable != tiles_are_passable)
        map.setDoorwayTiles(doorway, should_be_passable);
    }
  }

  void MazeShifter::openDoorwaysNear(
    TileMapModel &map,
    Vec2Props     position
  ) {
    for(DoorwayProps &doorway : map.doorways()) {
      if(doorway.is_open) continue;
      if(doorway.center().distance(position) > nearby_opening_reach) continue;

      doorway.is_open              = true;
      doorway.seconds_since_change = 0;
    }
  }

  bool MazeShifter::advance(
    TileMapModel              &map,
    RandomGenerator           &random,
    std::uint32_t              phase,
    float                      delta_seconds,
    Vec2Props                  view_center,
    std::optional<std::size_t> focus_room,
    std::span<const Vec2Props> occupants
  ) {
    moved_doorway_indices_ = {};
    timer_seconds_ -= delta_seconds;
    if(timer_seconds_ > 0) return false;

    const float reduction
      = interval_reduction_per_phase * static_cast<float>(std::min(phase, 6U));
    timer_seconds_
      = random.range(minimum_interval_seconds, maximum_interval_seconds)
      - reduction;

    if(!focus_room) return false;
    std::optional<std::size_t> opened;

    const std::vector<std::size_t> closed_candidates
      = collectCandidates(map, *focus_room, view_center, occupants, false);
    if(!closed_candidates.empty()) {
      opened = closed_candidates[random.index(closed_candidates.size())];
      map.doorways()[*opened].is_open              = true;
      map.doorways()[*opened].seconds_since_change = 0;
    }

    std::optional<std::size_t>     closed;
    const std::vector<std::size_t> open_candidates
      = collectCandidates(map, *focus_room, view_center, occupants, true);

    for(std::size_t attempt = 0; attempt < open_candidates.size(); ++attempt) {
      const std::size_t doorway_index
        = open_candidates[random.index(open_candidates.size())];
      if(opened && doorway_index == *opened) continue;

      DoorwayProps &doorway = map.doorways()[doorway_index];
      doorway.is_open       = false;

      if(TileMapConnectivityAnalyzer::isRoomGraphConnected(map)) {
        doorway.seconds_since_change = 0;
        closed                       = doorway_index;
        break;
      }

      doorway.is_open = true;
    }

    moved_doorway_indices_ = {opened, closed};
    return opened.has_value() || closed.has_value();
  }

  std::vector<std::size_t> MazeShifter::collectCandidates(
    const TileMapModel        &map,
    std::size_t                focus_room,
    Vec2Props                  view_center,
    std::span<const Vec2Props> occupants,
    bool                       wants_open
  ) {
    std::vector<std::size_t> candidates;
    const auto              &doorways = map.doorways();

    for(
      std::size_t doorway_index = 0; doorway_index < doorways.size();
      ++doorway_index
    ) {
      if(candidates.size() >= candidate_capacity) break;
      const DoorwayProps &doorway = doorways[doorway_index];

      if(
        doorway.is_open
        != wants_open
        || doorway.seconds_since_change
        < recent_change_seconds
      )
        continue;

      const int nearest_distance = std::min(
        map.roomGridDistance(doorway.room_a, focus_room),
        map.roomGridDistance(doorway.room_b, focus_room)
      );

      if(nearest_distance > neighborhood_grid_distance) continue;
      if(
        isInsideView(doorway.center(), view_center)
        || isOccupied(doorway, occupants)
      )
        continue;

      candidates.push_back(doorway_index);
    }

    return candidates;
  }

  bool MazeShifter::isOccupied(
    const DoorwayProps        &doorway,
    std::span<const Vec2Props> occupants
  ) {
    const float reach
      = (RenderConfig::tile_size * static_cast<float>(DoorwayProps::length) / 2)
      + occupant_clearance;

    return std::ranges::any_of(occupants, [&](Vec2Props occupant) {
      return occupant.distance(doorway.center()) < reach;
    });
  }

  bool MazeShifter::isInsideView(
    Vec2Props point,
    Vec2Props view_center
  ) {
    const float half_width
      = (static_cast<float>(RenderConfig::view_width) / 2) + hidden_margin;
    const float half_height
      = (static_cast<float>(RenderConfig::view_height) / 2) + hidden_margin;

    return std::abs(point.x - view_center.x)
      < half_width
      && std::abs(point.y - view_center.y)
      < half_height;
  }
} // namespace wheel_of_fear
