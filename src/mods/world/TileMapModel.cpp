#include "mods/world/TileMapModel.hpp"
#include "core/render/RenderConfig.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <utility>

namespace wheel_of_fear {
  void TileMapModel::reset(
    int width,
    int height
  ) {
    const auto cell_count
      = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);

    width_  = width;
    height_ = height;
    walls_.assign(cell_count, 1);
    structural_.assign(cell_count, 1);
    blocked_.assign(cell_count, 0);
    fluids_.reset(cell_count);
    room_indices_.assign(cell_count, -1);
  }

  void TileMapModel::carveFloor(
    int         x,
    int         y,
    std::size_t room_index
  ) {
    const std::size_t cell = cellIndex(x, y);
    walls_[cell]           = 0;
    structural_[cell]      = 0;
    room_indices_[cell]    = static_cast<std::int16_t>(room_index);
  }

  void TileMapModel::raiseWall(
    int x,
    int y
  ) {
    walls_[cellIndex(x, y)] = 1;
  }

  void TileMapModel::sealTile(
    int x,
    int y
  ) {
    const std::size_t cell = cellIndex(x, y);
    walls_[cell]           = 1;
    structural_[cell]      = 1;
    room_indices_[cell]    = -1;
  }

  void TileMapModel::setRooms(std::vector<RoomProps> rooms) {
    rooms_ = std::move(rooms);
  }

  void TileMapModel::setDoorwayTiles(
    const DoorwayProps &doorway,
    bool                is_passable
  ) {
    for(int step = 0; step < DoorwayProps::length; ++step) {
      const TileCoordinateProps tile = doorway.tileAt(step);
      const std::size_t         cell = cellIndex(tile.x, tile.y);

      walls_[cell] = is_passable ? 0 : 1;
      if(!is_passable) fluids_.clearCell(cell);
    }

    markChanged();
  }

  bool TileMapModel::isNearDoorway(
    int x,
    int y,
    int margin
  ) const {
    for(const DoorwayProps &doorway : doorways_) {
      for(int step = 0; step < DoorwayProps::length; ++step) {
        const TileCoordinateProps tile = doorway.tileAt(step);

        if(std::abs(tile.x - x) <= margin && std::abs(tile.y - y) <= margin)
          return true;
      }
    }

    return false;
  }

  bool TileMapModel::isAreaFree(
    int x,
    int y,
    int area_width,
    int area_height
  ) const {
    for(int check_y = y; check_y < y + area_height; ++check_y) {
      for(int check_x = x; check_x < x + area_width; ++check_x)
        if(isBlocked(check_x, check_y) || !roomIndexAtTile(check_x, check_y))
          return false;
    }

    return true;
  }

  bool TileMapModel::reserveProp(
    int x,
    int y,
    int area_width,
    int area_height
  ) {
    if(!isAreaFree(x, y, area_width, area_height)) return false;

    for(int check_y = y; check_y < y + area_height; ++check_y) {
      for(int check_x = x; check_x < x + area_width; ++check_x)
        if(isNearDoorway(check_x, check_y, 2)) return false;
    }

    for(int fill_y = y; fill_y < y + area_height; ++fill_y)
      for(int fill_x = x; fill_x < x + area_width; ++fill_x)
        blocked_[cellIndex(fill_x, fill_y)] = 1;

    markChanged();
    return true;
  }

  void TileMapModel::releaseProp(
    int x,
    int y,
    int area_width,
    int area_height
  ) {
    for(int fill_y = y; fill_y < y + area_height; ++fill_y) {
      for(int fill_x = x; fill_x < x + area_width; ++fill_x)
        if(contains(fill_x, fill_y)) blocked_[cellIndex(fill_x, fill_y)] = 0;
    }

    markChanged();
  }

  std::optional<TileCoordinateProps>
  TileMapModel::nearestOpenTile(TileCoordinateProps origin) const {
    if(!isBlocked(origin.x, origin.y)) return origin;

    for(int radius = 1; radius <= 8; ++radius) {
      for(int y = origin.y - radius; y <= origin.y + radius; ++y) {
        for(int x = origin.x - radius; x <= origin.x + radius; ++x)
          if(!isBlocked(x, y)) return TileCoordinateProps {.x = x, .y = y};
      }
    }

    return std::nullopt;
  }

  Vec2Props TileMapModel::nearestFloor(Vec2Props point) const {
    const TileCoordinateProps origin = tileAt(point);
    if(!isBlocked(origin.x, origin.y)) return point;

    for(int radius = 1; radius <= 8; ++radius) {
      std::optional<Vec2Props> best;
      float best_distance = std::numeric_limits<float>::infinity();

      for(int y = origin.y - radius; y <= origin.y + radius; ++y) {
        for(int x = origin.x - radius; x <= origin.x + radius; ++x) {
          if(isBlocked(x, y)) continue;
          const Vec2Props candidate = tileCenter(x, y);
          const float     distance  = candidate.distance(point);

          if(distance < best_distance) {
            best_distance = distance;
            best          = candidate;
          }
        }
      }
      if(best) return *best;
    }

    return point;
  }

  bool TileMapModel::isInterior(
    int x,
    int y
  ) const {
    return x >= 1 && y >= 1 && x < width_ - 1 && y < height_ - 1;
  }

  bool TileMapModel::contains(
    int x,
    int y
  ) const {
    return x >= 0 && y >= 0 && x < width_ && y < height_;
  }

  std::size_t TileMapModel::cellIndex(
    int x,
    int y
  ) const {
    return (static_cast<std::size_t>(y) * static_cast<std::size_t>(width_))
      + static_cast<std::size_t>(x);
  }

  bool TileMapModel::isWall(
    int x,
    int y
  ) const {
    if(!contains(x, y)) return true;
    return walls_[cellIndex(x, y)] != 0;
  }

  bool TileMapModel::isStructural(
    int x,
    int y
  ) const {
    if(!contains(x, y)) return true;
    return structural_[cellIndex(x, y)] != 0;
  }

  bool TileMapModel::isBlocked(
    int x,
    int y
  ) const {
    if(!contains(x, y)) return true;
    const std::size_t cell = cellIndex(x, y);
    return walls_[cell] != 0 || blocked_[cell] != 0;
  }

  TileCoordinateProps TileMapModel::tileAt(Vec2Props point) {
    return TileCoordinateProps {
      .x = static_cast<int>(std::floor(point.x / RenderConfig::tile_size)),
      .y = static_cast<int>(std::floor(point.y / RenderConfig::tile_size)),
    };
  }

  Vec2Props TileMapModel::tileCenter(
    int x,
    int y
  ) {
    return Vec2Props::of(
      (static_cast<float>(x) + 0.5F) * RenderConfig::tile_size,
      (static_cast<float>(y) + 0.5F) * RenderConfig::tile_size
    );
  }

  bool TileMapModel::isSolidAt(Vec2Props point) const {
    const TileCoordinateProps tile = tileAt(point);
    return isBlocked(tile.x, tile.y);
  }

  bool TileMapModel::isWallAt(Vec2Props point) const {
    const TileCoordinateProps tile = tileAt(point);
    return isWall(tile.x, tile.y);
  }

  bool TileMapModel::dissolveWall(
    int x,
    int y
  ) {
    if(!isInterior(x, y) || !isWall(x, y)) return false;

    const std::size_t cell = cellIndex(x, y);
    if(structural_[cell] != 0) return false;

    walls_[cell] = 0;
    markChanged();

    return true;
  }

  bool TileMapModel::collidesWithBox(
    Vec2Props center,
    float     half_size
  ) const {
    return isSolidAt(Vec2Props::of(center.x - half_size, center.y - half_size))
      || isSolidAt(Vec2Props::of(center.x + half_size, center.y - half_size))
      || isSolidAt(Vec2Props::of(center.x - half_size, center.y + half_size))
      || isSolidAt(Vec2Props::of(center.x + half_size, center.y + half_size));
  }

  Vec2Props TileMapModel::slide(
    Vec2Props position,
    Vec2Props displacement,
    float     half_size
  ) const {
    Vec2Props current = position;

    const auto step_count
      = static_cast<std::size_t>(std::ceil(displacement.length() / 4.0F) + 1);
    const Vec2Props step
      = displacement.scale(1.0F / static_cast<float>(step_count));

    for(std::size_t index = 0; index < step_count; ++index) {
      const Vec2Props horizontal = Vec2Props::of(current.x + step.x, current.y);
      if(!collidesWithBox(horizontal, half_size)) current = horizontal;

      const Vec2Props vertical = Vec2Props::of(current.x, current.y + step.y);
      if(!collidesWithBox(vertical, half_size)) current = vertical;
    }

    return current;
  }

  bool TileMapModel::hasLineOfSight(
    Vec2Props from,
    Vec2Props to
  ) const {
    const auto step_count
      = static_cast<std::size_t>(std::max(to.distance(from) / 5.0F, 1.0F));
    const TileCoordinateProps destination_tile = tileAt(to);

    for(std::size_t step = 1; step < step_count; ++step) {
      const TileCoordinateProps tile = tileAt(
        from.lerp(to, static_cast<float>(step) / static_cast<float>(step_count))
      );
      if(tile == destination_tile) return true;
      if(isWall(tile.x, tile.y)) return false;
    }

    return true;
  }

  Vec2Props TileMapModel::randomFloorInRoom(
    RandomGenerator &random,
    const RoomProps &room
  ) const {
    for(int attempt = 0; attempt < 60; ++attempt) {
      const int x
        = random.intInclusive(room.left + 1, room.left + room.width - 2);
      const int y
        = random.intInclusive(room.top + 1, room.top + room.height - 2);

      if(
        !isBlocked(x, y)
        && !isBlocked(x + 1, y)
        && !isBlocked(x - 1, y)
        && !isBlocked(x, y + 1)
        && !isBlocked(x, y - 1)
      )
        return tileCenter(x, y);
    }

    return nearestFloor(room.center());
  }

  std::optional<std::size_t> TileMapModel::roomIndexAtTile(
    int x,
    int y
  ) const {
    if(!contains(x, y)) return std::nullopt;
    const std::int16_t room_index = room_indices_[cellIndex(x, y)];

    if(room_index < 0) return std::nullopt;
    return static_cast<std::size_t>(room_index);
  }

  std::optional<std::size_t> TileMapModel::roomIndexAt(Vec2Props point) const {
    const TileCoordinateProps tile = tileAt(point);
    return roomIndexAtTile(tile.x, tile.y);
  }

  int TileMapModel::roomGridDistance(
    std::size_t first_room,
    std::size_t second_room
  ) const {
    return rooms_[first_room].gridDistance(rooms_[second_room]);
  }
} // namespace wheel_of_fear
